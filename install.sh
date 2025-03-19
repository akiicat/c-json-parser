# Formatting
sudo apt update
sudo apt install -y clang-format

# Meson
sudo apt-get install -y python3 python3-pip python3-setuptools \
                       python3-wheel ninja-build

pip3 install meson

meson wrap install gtest

# meson wrap install unity
# gem install ceedling
# Coverage
# pip install gcovr