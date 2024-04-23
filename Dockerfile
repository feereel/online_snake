FROM ubuntu

RUN apt update -y && \
    apt install build-essential libncurses5-dev libncursesw5-dev make -y
    
WORKDIR /app
COPY . .

RUN make

ENTRYPOINT [ "bash" ]