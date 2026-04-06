"""A Python sketch mirroring test/cached_execution.cpp function calls.

This file intentionally uses small stubs so the call structure mirrors the C++ test
without implementing full framework behavior.
"""

from dataclasses import dataclass
from typing import List, Mapping, Tuple

from framework import (
    Concurrency,
    DummyFrameworkGraph,
    LayerGenerator,
    product_query,
)


@dataclass
class Geometry:
    # simple placeholder geometry type for the demonstrator
    vertices: List[Tuple[float, float]]


def provide_geometry(index: Mapping[str, int]) -> Geometry:
    # produce a trivial geometry derived from the index value
    n = index["number"]
    verts = [(float(n), 0.0), (0.0, float(n)), (float(n), float(n))]
    return Geometry(vertices=verts)


def provide_string(index: Mapping[str, int]) -> str:
    # return a string representation derived from the input index
    return str(3 * index["number"])


def provide_integer(index: Mapping[str, int]) -> int:
    return 2 * index["number"]


def geometry_to_integer(geom: Geometry) -> int:
    # return the number of vertices
    return len(geom.vertices)


def integer_to_float(i: int) -> float:
    return float(i)


def float_to_float(x: float) -> float:
    return x + 1.0


def integer_and_string_to_float(i: int, s: str) -> float:
    # return a value derived from the geometry and string inputs
    return i * float(s)


def two_floats_to_float(x: float, y: float) -> float:
    return x + y


def float_and_integer_to_float(x: float, i: int) -> float:
    return x * i


def main():
    n_runs = 1
    n_subruns = 2
    n_events = 5000

    gen = LayerGenerator()
    gen.add_layer("run", ("job", n_runs))
    gen.add_layer("subrun", ("run", n_subruns))
    gen.add_layer("event", ("subrun", n_events))

    g = DummyFrameworkGraph(gen)

    # Register providers (also declare their output products)
    g.provide("provide_geometry", provide_geometry, Concurrency.unlimited).output_product(
        creator="provide_geometry", layer="run", suffix="number"
    )
    g.provide("provide_string", provide_string, Concurrency.unlimited).output_product(
        creator="provide_string", layer="subrun", suffix="another"
    )
    g.provide("provide_integer", provide_integer, Concurrency.unlimited).output_product(
        creator="provide_integer", layer="event", suffix="still"
    )

    g.transform("A1", geometry_to_integer, Concurrency.unlimited).input_family(
        product_query(creator="provide_geometry", layers="run", suffix="number")
    ).output_product_suffixes("a1_output")

    g.transform("A2", integer_to_float, Concurrency.unlimited).input_family(
        product_query(creator="A1", layers="run", suffix="a1_output")
    ).output_product_suffixes("a2_output")

    g.transform("A3", float_to_float, Concurrency.unlimited).input_family(
        product_query(creator="A2", layers="run", suffix="a2_output")
    ).output_product_suffixes("a3_output")

    g.transform("B1", integer_and_string_to_float, Concurrency.unlimited).input_family(
        product_query(creator="A1", layers="run", suffix="a1_output"),
        product_query(creator="provide_string", layers="subrun", suffix="another"),
    ).output_product_suffixes("b1_output")

    g.transform("B2", two_floats_to_float, Concurrency.unlimited).input_family(
        product_query(creator="A2", layers="run", suffix="a2_output"),
        product_query(creator="B1", layers="subrun", suffix="b1_output"),
    ).output_product_suffixes("b2_output")

    g.transform("C", float_and_integer_to_float, Concurrency.unlimited).input_family(
        product_query(creator="B2", layers="subrun", suffix="b2_output"),
        product_query(creator="provide_integer", layers="event", suffix="still"),
    ).output_product_suffixes("c_output")

    for node_name, layers in g.output_product_layers.items():
        print(f"Node '{node_name}': {layers}")


if __name__ == "__main__":
    main()
