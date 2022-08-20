#ifndef UTIL_H
#define UTIL_H

#include <iostream>
#include <vector>
#include <fstream>
#include <filesystem>
#include <string>
#include <algorithm>
#include <memory>
#include <map>
#include <array>
#include <sys/ioctl.h>
#include <unistd.h>

#define STB_IMAGE_IMPLEMENTATION
#define STB_IMAGE_WRITE_IMPLEMENTATION
#define STB_IMAGE_RESIZE_IMPLEMENTATION
#include "stb_image.h"
#include "stb_image_write.h"
#include "stb_image_resize.h"

#include "matplotlibcpp.h" /// https://github.com/lava/matplotlib-cpp

#endif