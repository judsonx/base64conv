/*
Base64 Converter 2018-07-03
Author: Judson Weissert

Copyright 2018 Judson Weissert

Permission is hereby granted, free of charge, to any person obtaining a copy of
this software and associated documentation files (the "Software"), to deal in
the Software without restriction, including without limitation the rights to
use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies
of the Software, and to permit persons to whom the Software is furnished to do
so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.
*/

#include <iostream>
#include <string>
#include <fstream>
#include <memory>

namespace base64conv
{

static size_t
get_next_triple(std::istream &s, uint8_t *buffer)
{
  size_t result = 0;
  bool ok = true;
  for (size_t i = 0; i < 3; ++i)
  {
    char x;
    if (ok && s.get(x))
    {
      result = i + 1;
      buffer[i] = (uint8_t) x;
    }
    else
    {
      ok = false;
      buffer[i] = '\0';
    }
  }
  return result;
}

static char const * const b64 =
  "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/";

int
usage(int argc, char *argv[])
{
  std::cerr << "Usage: " << argv[0] << " <path_to_file>"  << std::endl;
  std::cerr << "       Where <path_to_file> can be \"-\" for STDIN"  << std::endl << std::endl;

  return 1;
}

class input_provider
{
private:
  std::unique_ptr<std::ifstream> _fh;

public:
  input_provider(std::string path)
  {
    static const char *STDIN_SENTINEL = "-";
    if (path != STDIN_SENTINEL)
      _fh = std::make_unique<std::ifstream>(path, std::ifstream::binary);
  }

  std::istream &stream()
  {
    return _fh ? *_fh : std::cin;
  }
};

static int main(int argc, char *argv[])
{
  static const char PAD = '=';

  if (argc != 2)
    return usage(argc, argv);

  input_provider ip(argv[1]);

  uint8_t b[3];
  uint8_t out[4];

  for (;;)
  {
    size_t n = get_next_triple(ip.stream(), b);
    if (n == 0)
      break;

    out[0] = b[0] >> 2;
    out[1] = ((b[0] & 0x03) << 4) + (b[1] >> 4);
    out[2] = ((b[1] & 0x0F) << 2) + ((b[2] & 0xC0) >> 6);
    out[3] = b[2] & 0x3F;

    for (size_t i = 0; i < 4; ++i)
    {
      if (i < n + 1)
        std::cout << b64[out[i]];
      else
        std::cout << PAD;
    }

    if (n < 3)
      break;
  }

  std::cout << std::endl;
  return 0;
}

} // namespace base64conv

int
main(int argc, char *argv[])
{
  return base64conv::main(argc, argv);
}
