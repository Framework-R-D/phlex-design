FROM registry.gitlab.com/islandoftex/images/texlive:TL2025-2025-04-27-full
RUN curl -fL -o /tmp/Miniforge3.sh \
      https://github.com/conda-forge/miniforge/releases/latest/download/Miniforge3-Linux-x86_64.sh \
    && bash /tmp/Miniforge3.sh -b -p /root/miniforge3 \
    && rm /tmp/Miniforge3.sh

ENV PATH="/root/miniforge3/bin:$PATH"
RUN conda install --yes --name base --override-channels -c conda-forge \
      python=3.13 pip doxygen "graphviz>=12"
RUN python -m pip install \
      "sphinx>=9.1,<10" "breathe>=4.36,<5" \
      "sphinx-needs==9.2.0" graphviz2drawio \
    && python -m pip check
