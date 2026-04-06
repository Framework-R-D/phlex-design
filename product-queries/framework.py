import copy
import inspect
from collections import defaultdict
from itertools import chain
from typing import Dict, NamedTuple

import dataset


class ConsumerPort(NamedTuple):
    node: str
    param_name: str


class Concurrency:
    unlimited = "unlimited"


def product_query(
    creator: str = None, layers: str | list[str] = None, suffix: str = None
) -> Dict[str, str]:
    query = {}
    if creator:
        query["creator"] = creator
    if layers:
        query["layers"] = [layers] if isinstance(layers, str) else layers
    if suffix:
        query["suffix"] = suffix
    return query


class LayerGenerator:
    def __init__(self):
        self._ancestors = defaultdict(set)

    def add_layer(self, name, pair):
        parent = pair[0]
        self._ancestors[name] = {parent} | self._ancestors[parent]

    def minimum_layers(self, layer_names):
        # A layer is minimum (lowest) if none of its ancestors appear in layer_names
        layer_names_set = set(layer_names)
        all_ancestors = set(chain.from_iterable(self._ancestors[n] for n in layer_names_set))
        return layer_names_set - all_ancestors


class TransformBuilder:
    def __init__(self, graph, name, func):
        self.graph = graph
        self.name = name
        self.func = func

    def input_family(self, *queries):
        # register each product_query entry into the graph inputs table
        # map queries to function parameters by position when possible
        sig = inspect.signature(self.func)
        param_names = list(sig.parameters.keys())
        for idx, pq in enumerate(queries):
            creator = pq.get("creator") if isinstance(pq, dict) else None
            suffix = pq.get("suffix") if isinstance(pq, dict) else None
            param_name = param_names[idx] if idx < len(param_names) else None
            # determine annotated type for the parameter
            ann = getattr(self.func, "__annotations__", {}).get(param_name, "unknown")
            param_type = getattr(ann, "__name__", str(ann))
            self.graph.register_input_product(
                node_name=self.name,
                creator=creator,
                suffix=suffix,
                param_name=param_name,
                arg_type=param_type,
            )
        return self

    def output_product_suffixes(self, *suffixes):
        # register an output product row for each suffix
        # infer data_type from the transform function's return annotation if available
        ann = getattr(self.func, "__annotations__", {}).get("return", "unknown")
        inferred = getattr(ann, "__name__", str(ann))
        for s in suffixes:
            self.graph.register_output(data_type=inferred, node_name=self.name, suffix=s)
        return self


class DummyFrameworkGraph:
    def __init__(self, layer_gen):
        self._layer_gen = layer_gen
        self._output_product_layers = {}
        # dataset in-memory DB for ergonomic queries
        self._db = dataset.connect("sqlite:///:memory:")
        # outputs table (raw dataset table) and normalized layers table
        self._outputs = self._db["outputs"]
        self._tbl_layers = self._db["output_layers"]
        self._input_queries = self._db["inputs"]

    def provide(self, name, func, concurrency):
        # return a small proxy that allows callers to register the provider's output product
        self_name = name

        def output_product(creator=None, layer=None, suffix=None):
            # infer data_type from the registered function's return annotation
            ann = getattr(func, "__annotations__", {}).get("return", "unknown")
            data_type = getattr(ann, "__name__", str(ann))
            self.register_output(
                data_type=data_type, node_name=self_name, suffix=suffix, layers=[layer]
            )
            return None

        class _P:
            def output_product(self, creator=None, layer=None, suffix=None):
                return output_product(creator=creator, layer=layer, suffix=suffix)

        return _P()

    def register_output(self, data_type, node_name, suffix, layers=[]):
        row = {
            "node_name": node_name,
            "data_type": data_type,
            "suffix": suffix,
        }
        # insert output row and then normalize layers into separate table
        out_id = self._outputs.insert(row)

        # populate normalized layers table
        for L in layers:
            self._tbl_layers.insert({"output_id": out_id, "layer": L})

    def register_input_product(self, node_name, creator, suffix, param_name, arg_type):
        row = {
            "node_name": node_name,
            "creator": creator,
            "suffix": suffix,
            "param_name": param_name,
            "arg_type": arg_type,
        }
        self._input_queries.insert(row)

    def transform(self, name, func, concurrency):
        return TransformBuilder(self, name, func)

    def _layers_for_output(self, out_id):
        return {lrow["layer"] for lrow in self._tbl_layers.find(output_id=out_id)}

    def compute_layers(self):
        """Compute the set of layers for each producer node in the graph.

        Returns a dict mapping node name to its set of layers.
        """
        # Build edges from each registered input query to matching output products
        edges = []

        for row in self._input_queries.all():
            # We intentionally do not filter by layer here: the layer of a transform
            # output is not known until layers are propagated through the graph below.
            clauses = {"data_type": row["arg_type"]}
            if row["creator"]:
                clauses["node_name"] = row["creator"]
            if row["suffix"]:
                clauses["suffix"] = row["suffix"]

            for r in self._outputs.find(**clauses):
                edges.append(
                    (
                        r["id"],
                        r["node_name"],
                        ConsumerPort(row["node_name"], row["param_name"]),
                    )
                )

        # Assign any layers that have been explicitly inserted into the layers table
        node_to_out_id = {node_name: out_id for out_id, node_name, _ in edges}
        output_layers = defaultdict(
            set,
            {
                node_name: self._layers_for_output(out_id)
                for node_name, out_id in node_to_out_id.items()
            },
        )

        # Build a map from each producer to the consumer ports that depend on it
        producer_to_ports = defaultdict(list)
        for _, producer_name, port in edges:
            producer_to_ports[producer_name].append(port)

        # Propagate layers through the graph using a worklist: when a producer's
        # layers become known, immediately propagate to its consumers.
        worklist = [name for name in output_layers if output_layers[name]]
        while worklist:
            producer_name = worklist.pop()
            for port in producer_to_ports.pop(producer_name, []):
                output_layers[port.node].update(output_layers[producer_name])
                worklist.append(port.node)

        # Apply minimum layers filter to each node's layers: only the lowest layer(s) should be retained
        for node_name, layers in output_layers.items():
            output_layers[node_name] = self._layer_gen.minimum_layers(layers)

        return output_layers

    @property
    def output_product_layers(self):
        if not self._output_product_layers:
            self._output_product_layers = self.compute_layers()
        return self._output_product_layers
