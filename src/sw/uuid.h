////////////////////////////////////////////////////////////////////////////////
/// Copyright 2018 Steven C. Wilson
///
/// Permission is hereby granted, free of charge, to any person obtaining a copy of this software
/// and associated documentation files (the "Software"), to deal in the Software without
/// restriction, including without limitation the rights to use, copy, modify, merge, publish,
/// distribute, sublicense, and/or sell copies of the Software, and to permit persons to whom the
/// Software is furnished to do so, subject to the following conditions:
///
/// The above copyright notice and this permission notice shall be included in all copies or
/// substantial portions of the Software.
///
/// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING
/// BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND
/// NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM,
/// DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
/// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
////////////////////////////////////////////////////////////////////////////////
#pragma once

#include "base64.h"
#include "system_traits.h"
#include "utils.h"

#include <array>
#include <iostream>

SW_NAMESPACE_BEGIN

template <typename SystemTraits = system::ThisSystemTraits>
class UuidType;
using Uuid = UuidType<>;

////////////////////////////////////////////////////////////////////////////////
/// Class that represents a UUID
////////////////////////////////////////////////////////////////////////////////
template <typename SystemTraits>
class UuidType {
public:
  /// Creates a random UUID
  static UuidType create() noexcept;

  /// Creates an invalid UUID
  UuidType() noexcept : bytes_({{0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0}}) {}

  /// Creates a UUID from the given bytes.
  explicit UuidType(const std::array<byte, 16>& bytes) noexcept : bytes_(bytes) {}

  /// Creates a UUID from the 16 bytes starting at the given byte pointer
  explicit UuidType(const byte* bytes) noexcept { ::memcpy(bytes_.data(), bytes, bytes_.size()); }

  // Copy/Move is standard
  UuidType(const UuidType& other) noexcept = default;
  UuidType& operator=(const UuidType& other) noexcept = default;
  UuidType(UuidType&& other) noexcept = default;
  UuidType& operator=(UuidType&& other) noexcept = default;

  // TODO Uuid(const StringWrapper& string);
  ////////////////////////////////////////////////////////////////////////////////
  const byte* bytes() const { return bytes_.data(); }
  constexpr sizex size() const { return 16; }

  ////////////////////////////////////////////////////////////////////////////////
  /// Convert the UUID into a string
  std::string toString() const {
    std::array<char, 36> buffer = {{0}};
    for (size_t i = 0, pos = 0; i < 16; ++i) {
      if (i == 4 || i == 6 || i == 8 || i == 10) {
        buffer[pos++] = '-';
      }
      auto hexChar = utils::toHexChar(bytes_[i]);
      buffer[pos++] = hexChar[0];
      buffer[pos++] = hexChar[1];
    }

    std::string result(buffer.data(), 36);
    return result;
  }

  ////////////////////////////////////////////////////////////////////////////////
  /// Convert the UUID into a base64 string
  std::string toBase64() const { return base64Encode(bytes_.data(), 16); }

  ////////////////////////////////////////////////////////////////////////////////
  /// Convert the UUID into a filename-safe base64 string
  std::string toBase64Url() const { return base64UrlEncode(bytes_.data(), 16); }

  ////////////////////////////////////////////////////////////////////////////////
  /// Determine if the UUID is valid or not. An invalid UUID has all bytes as 0
  bool isValid() const {
    auto invalid = Uuid();
    return *this != invalid;
  }

  friend bool operator==(const UuidType& a, const UuidType& b) { return a.bytes_ == b.bytes_; }
  friend bool operator!=(const UuidType& a, const UuidType& b) { return a.bytes_ != b.bytes_; }
  friend bool operator<(const UuidType& a, const UuidType& b) { return a.bytes_ < b.bytes_; }
  friend bool operator<=(const UuidType& a, const UuidType& b) { return a.bytes_ <= b.bytes_; }
  friend bool operator>(const UuidType& a, const UuidType& b) { return a.bytes_ > b.bytes_; }
  friend bool operator>=(const UuidType& a, const UuidType& b) { return a.bytes_ >= b.bytes_; }
  friend std::ostream& operator<<(std::ostream& outs, const UuidType& u) {
    return outs << u.toString().c_str();
  }

private:
  std::array<byte, 16> bytes_ = {{0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0}};
};

SW_NAMESPACE_END

////////////////////////////////////////////////////////////////////////////////
/// Per-OS implementation.
///
/// SW: I really don't like including OS headers here, but no other way to do this
///  for "header-only".
////////////////////////////////////////////////////////////////////////////////

#if SW_UNIX

#  include <uuid/uuid.h>
SW_NAMESPACE_BEGIN

////////////////////////////////////////////////////////////////////////////////
template <>
inline Uuid Uuid::create() noexcept {
  // NOTE: uuid_t is a uint8[16], so we can pass it directly to our constructor
  uuid_t id;
  ::uuid_generate(id);
  return Uuid(id);
}

#elif SW_WINDOWS
#  error Implement Uuid::create for windows
#endif

SW_NAMESPACE_END
