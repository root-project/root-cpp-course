FROM condaforge/miniforge3

ARG CONDA_ENV_NAME=root-cpp-course 
RUN conda create -n $CONDA_ENV_NAME root cxx-compiler

ENV PATH=/opt/conda/envs/$CONDA_ENV_NAME/bin:$PATH
ENV CONDA_DEFAULT_ENV=$CONDA_ENV_NAME

RUN echo "conda activate $CONDA_ENV_NAME" >> ~/.bashrc
