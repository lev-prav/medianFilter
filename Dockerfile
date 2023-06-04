FROM ubuntu

WORKDIR /medianFilter

# Скачаем все необходимые пакеты и выполним сборку GoogleTest
# Такая длинная команда обусловлена тем, что
# Docker на каждый RUN порождает отдельный слой,
# Влекущий за собой, в данном случае, ненужный оверхед
# Ставим OpenCV
RUN apt update;  \
    apt install -y g++ cmake make ninja-build git libgtk2.0-dev pkg-config

RUN git clone https://github.com/opencv/opencv.git ; \
    mkdir -p build && cd build ; \
    cmake ../opencv ;\
    make -j4 ;\
    make install


COPY . app/

RUN cmake -DCMAKE_BUILD_TYPE=Release -DCMAKE_MAKE_PROGRAM=ninja -G Ninja -S app -B app/cmake-build-release ; \
    cmake --build app/cmake-build-release

# Установим точку входа
ENTRYPOINT ["app/build/median_filter", "app/res/maxresdefault.jpg"]