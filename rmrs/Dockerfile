FROM gcc:12-bookworm

# Установка системных зависимостей
RUN apt-get update && apt-get install -y \
    cmake \
    git \
    && rm -rf /var/lib/apt/lists/*

# Установка Catch2 v3.4.0 (стабильная версия)
WORKDIR /tmp
RUN git clone https://github.com/catchorg/Catch2.git \
    && cd Catch2 \
    && git checkout v3.4.0 \
    && cmake -B build -S . -DBUILD_TESTING=OFF \
    && cmake --build build --target install \
    && cd .. \
    && rm -rf Catch2

# Рабочая директория для проекта
WORKDIR /workspace

COPY . .
RUN chmod +x build.sh /workspace/build.sh
# Команда по умолчанию - сборка и тесты
CMD ["./build.sh"]