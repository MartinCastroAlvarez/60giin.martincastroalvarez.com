#include "engine/texture.hpp"
#include "engine/shader.hpp"

#pragma warning(push, 0)
#include <glad/glad.h>
#include <iostream>
#include <stb_image.h>
#pragma warning(pop)

static uint32_t format2GL(Texture::Format format) {
  switch (format) {
    case Texture::Format::RGB: return GL_RGB;
    case Texture::Format::RGBA: return GL_RGBA;
    default:return GL_RGB;
  };
}

Texture::Texture(const char* path, Format format) : _format(format) {

  stbi_set_flip_vertically_on_load(true);

  glGenTextures(1, &_id);
  glBindTexture(GL_TEXTURE_2D, _id);

  applyWrapping();
  applyFilter();

  unsigned char* data = stbi_load(path, &_width, &_height, &_depth, 0);
  if (data) {
    glTexImage2D(GL_TEXTURE_2D, 0, format2GL(_format), _width, _height, 0, format2GL(_format), GL_UNSIGNED_BYTE, data);
    glGenerateMipmap(GL_TEXTURE_2D);
    stbi_image_free(data);
  }
  else {
    std::cout << "Failed to Load Texture " << path << std::endl;
  }

  glBindTexture(GL_TEXTURE_2D, 0);
}

Texture::~Texture() {
  glDeleteTextures(1, &_id);
}

void Texture::setWrap(Wrap s, Wrap t) {
  _wrap = std::make_pair(s, t);
  applyWrapping();
}

void Texture::setFilter(Filter mag, Filter min, Filter mipMapMag, Filter mipMapMin) {
  _filter = std::make_pair(mag, min);
  _filterMipMap = std::make_pair(mipMapMag, mipMapMin);
  applyFilter();
}

static uint32_t wrap2GL(Texture::Wrap wrap) {
  switch (wrap) {
    case Texture::Wrap::Repeat: return GL_REPEAT;
    case Texture::Wrap::Mirrored_Repeat: return GL_MIRRORED_REPEAT;
    case Texture::Wrap::Clamp_To_Border: return GL_CLAMP_TO_BORDER;
    case Texture::Wrap::Clamp_To_Edge: return GL_CLAMP_TO_EDGE;
    default: return GL_REPEAT;
  };
}

void Texture::applyWrapping() const {
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, wrap2GL(_wrap.first));
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, wrap2GL(_wrap.second));
}

static uint32_t filter2GL(Texture::Filter filter, Texture::Filter mipMap) {
  switch (filter) {
  case Texture::Filter::Nearest: switch (mipMap) {
  case Texture::Filter::None: return GL_NEAREST;
  case Texture::Filter::Nearest: return GL_NEAREST_MIPMAP_NEAREST;
  case Texture::Filter::Linear: return GL_NEAREST_MIPMAP_LINEAR;
  default: return GL_NEAREST;
  };
  case Texture::Filter::Linear: switch (mipMap) {
  case Texture::Filter::None: return GL_LINEAR;
  case Texture::Filter::Nearest: return GL_LINEAR_MIPMAP_NEAREST;
  case Texture::Filter::Linear: return GL_LINEAR_MIPMAP_LINEAR;
  default: return GL_LINEAR;
  };
  case Texture::Filter::None:
  default: return GL_NEAREST;
  }
}

void Texture::applyFilter() const {
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, filter2GL(_filter.first, _filterMipMap.first));
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, filter2GL(_filter.second, _filterMipMap.second));
}

void Texture::use(const Shader& shader, const char* name, uint32_t unit) const {
  glActiveTexture(GL_TEXTURE0 + unit);
  glBindTexture(GL_TEXTURE_2D, _id);
  shader.set(name, static_cast<int32_t>(unit));
}