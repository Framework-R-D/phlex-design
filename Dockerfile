FROM registry.gitlab.com/islandoftex/images/texlive:TL2025-2025-04-27-full
RUN <<EOF
curl -O -L  https://github.com/conda-forge/miniforge/releases/latest/download/Miniforge3-Linux-x86_64.sh
bash Miniforge3-Linux-x86_64.sh -b
cat <<XXX >> /root/.bashrc
export MAMBA_EXE='/workdir/bin/mamba';
export MAMBA_ROOT_PREFIX='/workdir';
__mamba_setup="$("$MAMBA_EXE" shell hook --shell bash --root-prefix "$MAMBA_ROOT_PREFIX" 2> /dev/null)"
if [ $? -eq 0 ]; then
    eval "$__mamba_setup"
else
    alias mamba="$MAMBA_EXE"  # Fallback on help from mamba activate
fi
unset __mamba_setup
XXX
EOF

ENV PATH="/root/miniforge3/bin:$PATH"
RUN conda init
RUN <<EOF
conda activate base
conda install --yes "sphinx>=8.2" graphviz2drawio "graphviz>=12" sphinx-needs
EOF
