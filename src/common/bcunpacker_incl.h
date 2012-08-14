/**
\author		Alexey Shaydurov aka ASH
\date		28.04.2012 (c)Andrey Korotkov

This file is a part of DGLE2 project and is distributed
under the terms of the GNU Lesser General Public License.
See "DGLE2.h" for more details.
*/

template<bool>
struct IDX_BLOCK_TYPE;

template<>
struct IDX_BLOCK_TYPE<true>
{
	typedef uint64 TYPE;
};

template<>
struct IDX_BLOCK_TYPE<false>
{
	typedef uint32 TYPE;
};

template<uint8 bpi>
struct INDICES
{
	inline uint operator [](uint i) const
	{
		register union
		{
			IDX_BLOCK_TYPE<(block_bytes > sizeof(uint32))>::TYPE uint: block_bits;
			uint8 bytes[block_bytes];
		};
		for(uint8 byte_idx = 0; byte_idx < block_bytes; ++byte_idx)
			bytes[byte_idx] = data[byte_idx];
		return uint >> i * bpi & mask;
	}
private:
	static const uint8 block_bits = bpi * 16, block_bytes = block_bits / 8;
	static const uint mask = ~0u >> (sizeof(uint) * 8 - bpi);
	uint8 data[block_bytes];
};

struct COLOR24;

struct COLOR48
{
	inline COLOR48(uint16 r, uint16 g, uint16 b): r(r), g(g), b(b) {}
	inline COLOR24 operator /(uint8 scalar) const;
	uint16 r, g, b;
};

struct COLOR24
{
	inline COLOR24(uint16 color16): r(static_cast<uint8>((color16 >> 11 & 0x1f) * 255.f / 31.f)), g(static_cast<uint8>((color16 >> 5 & 0x3f) * 255.f / 63.f)), b(static_cast<uint8>((color16 >> 0 & 0x1f) * 255.f / 31.f)) {}
	inline COLOR24(uint8 r, uint8 g, uint8 b): r(r), g(g), b(b) {}
	inline operator COLOR48() const
	{
		return COLOR48(r, g, b);
	}
	inline COLOR48 operator *(uint8 scalar) const;
	uint8 r, g, b;
};

inline COLOR48 COLOR24::operator *(uint8 scalar) const
{
	return COLOR48(r * scalar, g * scalar, b * scalar);
}

inline COLOR24 COLOR48::operator /(uint8 scalar) const
{
	return COLOR24(r / scalar, g / scalar, b / scalar);
}

inline COLOR48 operator +(const COLOR48 &left, const COLOR48 &right)
{
	return COLOR48(left.r + right.r, left.g + right.g, left.b + right.b);
}

void UnpackBC1(uint16 width, uint16 height, const uint8 *src, uint8 *dst)
{
#pragma pack(push, 1)
	struct BC1_BLOCK
	{
		uint16 color_0, color_1;
		INDICES<2> indices;
	};
#pragma pack(pop)
	for (uint16 blockY = 0; blockY < (height + 3) / 4; ++blockY)
		for (uint16 blockX = 0; blockX < (width + 3) / 4; ++blockX)
		{
			const BC1_BLOCK &cur_block = reinterpret_cast<const BC1_BLOCK *>(src)[blockY * ((width + 3) / 4) + blockX];
			COLOR24 color_table[4] =
			{
				cur_block.color_0,
				cur_block.color_1,
				(color_table[0] * 2 + color_table[1]) / 3,
				(color_table[0] + color_table[1] * 2) / 3
			};
			for (int y = 0; y < 4 && y < height - blockY * 4; ++y)
				for (int x = 0; x < 4 && x < width - blockX * 4; ++x)
				{
					uint8 (&dst_pixel)[3] = reinterpret_cast<uint8 (*const&)[3]>(dst)[(blockY * 4 + y) * width + (blockX * 4 + x)];
					uint color_idx = cur_block.indices[y * 4 + x];
					dst_pixel[0] = color_table[color_idx].r;
					dst_pixel[1] = color_table[color_idx].g;
					dst_pixel[2] = color_table[color_idx].b;
				}
		}
}

void UnpackBC3(uint16 width, uint16 height, const uint8 *src, uint8 *dst)
{
#pragma pack(push, 1)
	struct BC3_BLOCK
	{
		uint8 alpha_0, alpha_1;
		INDICES<3> alpha_indices;
		uint16 color_0, color_1;
		INDICES<2> color_indices;
	};
#pragma pack(pop)
	for (uint16 blockY = 0; blockY < (height + 3) / 4; ++blockY)
		for (uint16 blockX = 0; blockX < (width + 3) / 4; ++blockX)
		{
			const BC3_BLOCK &cur_block = reinterpret_cast<const BC3_BLOCK *>(src)[blockY * ((width + 3) / 4) + blockX];
			COLOR24 color_table[4] =
			{
				cur_block.color_0,
				cur_block.color_1,
				(color_table[0] * 2 + color_table[1]) / 3,
				(color_table[0] + color_table[1] * 2) / 3
			};
			uint8 alpha_table[8] = {cur_block.alpha_0, cur_block.alpha_1};
			if (alpha_table[0] > alpha_table[1])
			{
				alpha_table[2] = (alpha_table[0] * 6 + alpha_table[1] * 1) / 7;
				alpha_table[3] = (alpha_table[0] * 5 + alpha_table[1] * 2) / 7;
				alpha_table[4] = (alpha_table[0] * 4 + alpha_table[1] * 3) / 7;
				alpha_table[5] = (alpha_table[0] * 3 + alpha_table[1] * 4) / 7;
				alpha_table[6] = (alpha_table[0] * 2 + alpha_table[1] * 5) / 7;
				alpha_table[7] = (alpha_table[0] * 1 + alpha_table[1] * 6) / 7;
			}
			else
			{
				alpha_table[2] = (alpha_table[0] * 4 + alpha_table[1] * 1) / 5;
				alpha_table[3] = (alpha_table[0] * 3 + alpha_table[1] * 2) / 5;
				alpha_table[4] = (alpha_table[0] * 2 + alpha_table[1] * 3) / 5;
				alpha_table[5] = (alpha_table[0] * 1 + alpha_table[1] * 4) / 5;
				alpha_table[6] = 0;
				alpha_table[7] = 255;
			}
			for (int y = 0; y < 4 && y < height - blockY * 4; ++y)
				for (int x = 0; x < 4 && x < width - blockX * 4; ++x)
				{
					uint8 (&dst_pixel)[4] = reinterpret_cast<uint8 (*const&)[4]>(dst)[(blockY * 4 + y) * width + (blockX * 4 + x)];
					uint color_idx = cur_block.color_indices[y * 4 + x];
					dst_pixel[0] = color_table[color_idx].r;
					dst_pixel[1] = color_table[color_idx].g;
					dst_pixel[2] = color_table[color_idx].b;
					dst_pixel[3] = alpha_table[cur_block.alpha_indices[y * 4 + x]];
				}
		}
}