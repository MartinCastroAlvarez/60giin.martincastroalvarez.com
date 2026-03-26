#pragma once

#include <cstdint>
#include <utility>

class Shader;

class Texture {
public:
  enum class Wrap {
    Repeat,
    Mirrored_Repeat,
    Clamp_To_Border,
    Clamp_To_Edge
  };

  enum class Filter {
    None,
    Nearest,
    Linear
  };

  enum class Format {
    RGB,
    RGBA
  };

  Texture() = delete;
  Texture(const char* path, Format format);
  ~Texture();

  Texture(const Texture&) = default;
  Texture& operator=(const Texture&) = default;

  Texture(Texture&&) noexcept = default;
  Texture& operator=(Texture&&) noexcept = default;

  void use(const Shader& shader, const char* name, uint32_t unit = 0) const;

  void setWrap(Wrap s, Wrap t);
  void setFilter(Filter mag, Filter min, Filter mipMapMag = Filter::None, Filter mipMapMin = Filter::None);

private:
  void applyWrapping() const;
  void applyFilter() const;

private:
  uint32_t _id = 0;
  Format _format = Format::RGB;
  int32_t _width = 0, _height = 0;
  int32_t _depth = 0;
  std::pair<Wrap, Wrap> _wrap{ Wrap::Repeat, Wrap::Repeat };
  std::pair<Filter, Filter> _filter{ Filter::Linear, Filter::Linear };
  std::pair<Filter, Filter> _filterMipMap{ Filter::None, Filter::None };
};