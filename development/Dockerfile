# So instead we're forcing a 32-bit image.
FROM python:3.10

# use `install_packages` if you need to install dependencies,
# for instance if you need git, just uncomment the line below.
COPY ./c_project/lib/libIDTechSDK.so /usr/local/lib/
RUN apt-get update \
    && apt-get upgrade -y --no-install-recommends \
    && apt-get install -y --no-install-recommends \
        git g++ curl libusb-1.0-0-dev build-essential
RUN ldconfig

ENV PYTHONUNBUFFERED 1

COPY ./requirements/base.txt /code/base.txt
COPY ./requirements/test.txt /code/test.txt
RUN pip install -Ur /code/test.txt

COPY . /code/
WORKDIR /code/

# Enable udevd so that plugged dynamic hardware devices show up in our container.
ENV UDEV=1

# main.py will run when container starts up on the device
CMD ["python", "-m", "src.runner"]
