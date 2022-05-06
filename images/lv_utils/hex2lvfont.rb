# coding: utf-8
#
# Copyright (c) 2017-2019  Stéphane D'Alu
# SPDX-License-Identifier: Apache-2.0
#
#
# Generates pixel perfect lv_font from hex font
# For LittlevGL >= 6
# 
# Availlable HEX fonts are:
#  * http://pelulamu.net/unscii/
#  * http://unifoundry.com/unifont/index.html
#
# Usage example:
#   ruby hex2lvfont.rb --help
#   ruby hex2lvfont.rb unscii-16.hex --height 16
#


require 'optparse'
require 'set'

begin
    require 'unicode_utils/char_name'
rescue LoadError
    warn "To get unicode names, you can install the gem 'unicode_utils'"
end


#
# Glyphs
#
class FontConverter
class Glyph
    class Error < StandardError
    end
        
    HEX_REGEXP = /^(?<code>[0-9a-f]+):(?<data>[0-9a-f]+)$/i

    attr_reader :height
    attr_reader :width
    attr_reader :bpp
    attr_reader :code

    # Convert data string to glyph
    def self.from_hex(data, height:)
        unless m = HEX_REGEXP.match(data)
            raise Error, "failed to decode hex glyph"
        end
            
        code       = m[:code].to_i(16)
        data       = [ m[:data] ].pack('H*').unpack1("B*")
                                 .each_char.map {|v| v.to_i }
        width      = data.size / height
        data       = data.each_slice(width).map {|e| e }
        
        self.new(code, height, width, data, 1)
    end

    # Return list of glyphs from an hew file
    def self.from_hexfile(fontfile, height:)
        File.foreach(fontfile).with_index.map {|line, idx|
            begin
                Glyph.from_hex(line, height: height)
            rescue Glyph::Error
                warn "Ignoring glyph at #{idx} due to parsing error"
            end
        }.compact
    end

    # Instanciate a glyph
    def initialize(code, height, width, data, bpp)
        @code   = code
        @height = height
        @width  = width
        @data   = data
        @bpp    = bpp
    end

    # Convert a glyph to a text array for visual representation
    def to_text
        @data.map {|row| row.map {|e| e > 0 ? '#' : '.' }.join }
    end

    # Convert a glyph to a raw binary
    def to_bin
        bitstring  = @data.flat_map {|row| row.map {|v| "%0*b" % [ @bpp, v ] } }
                          .join
        [ bitstring ].pack('B*')
    end

    # Compute bounding box (origin is top-left)
    def bounding
        lines = @data.map {|row| row.any? {|v| !v.zero? } }
        if y = lines.find_index(true)
            h = lines.size - lines.reverse.find_index(true) - y
        else
            y = h = 0
        end
        
        rows = @data.transpose.map {|row| row.any? {|v| !v.zero? } }
        if x = rows.find_index(true)
            w = rows.size - rows.reverse.find_index(true) - x
        else
            x = w = 0
        end
        
        { x: x, y: y, w: w, h: h }
    end

    # Extract part of the glyph (generally use with bounding box)
    def extract(x:, y:, w:, h:)
        Glyph.new(@code, h, w, @data[y,h].map {|row| row[x,w] }, @bpp)
    end
end
end


#
# FontConverter
#   
class FontConverter
    def initialize(base_line: 0)
        @glyphs    = {}
        @list      = []
        @base_line = base_line
    end
    
    def import(*glyphs)
        glyphs.each {|g|
            @glyphs.merge!(g.bpp => { g.code => g }) {|k, o, n|
                o.merge(n) {|k, o, n|
                    warn "Overwriting existing glyph (U+%04X, bpp: %d)" % [
                             g.code, g.bpp ]
                    n
                }
            }
        }
    end

    def select(*codes)
        @list = codes.flat_map {|code|
            case code
            when Range, Set then code.to_a
            when String     then code.ord
            when Integer    then code
            else warn "Ignoring unhandled code description (#{code.class})"
            end
        }.compact.uniq.sort
    end

    def self.cmaps(list, cutoff: 10)
        # find contiguous glyphs
        cmaps = list.slice_when {|a,b| (a+1) != b }.map {|l| l.first .. l.last }
        # group isolated glyphs (less than cutoff value) into a sparse set
        cmaps.slice_when {|a,b| [a.size,b.size].max > cutoff }
             .map {|a| a.size > 1 ? a.flat_map(&:to_a) : a.first }
    end

    def build(name, bpp: 1, cmaps_cutoff: 10, source: :lvgl)
        glyphs = @glyphs[bpp]
        list   = (@list & glyphs.keys)             # Ensure that glyphs exist
                     .reject {|code| code < 32 }   # Reject control characters
        cmaps  = FontConverter.cmaps(list, cutoff: cmaps_cutoff)

        line_height = glyphs.values.map {|g| g.height}.max
        
        # Header
        puts case source
             when :zephyr then "#include <lvgl.h>"
             else              "#include <lvgl/lv_misc/lv_font.h>"
             end
        
        puts ""
        puts "/* See: https://github.com/littlevgl/lv_font_conv/blob/master/doc/font_spec.md"
        puts " */"
        puts ""
        puts "#ifndef LV_FONT_#{name.upcase}"
        puts "#define LV_FONT_#{name.upcase} 1"
        puts "#endif"
        puts ""

        puts "#if LV_FONT_#{name.upcase}"

        # Glyphs definition
        puts ""
        puts "/* Store the image of the glyphs"
        puts " */"
        puts "static LV_ATTRIBUTE_LARGE_CONST uint8_t glyph_bitmap[] = "
        puts "{"
        offset = 0
        list.each {|code| g = glyphs[code]
            bounding = g.bounding
            bin      = g.extract(bounding).to_bin
            unidesc  = if Object.const_defined?(:UnicodeUtils)
                           ' [' + UnicodeUtils.char_name(code) + ']'
                       end
            puts "  /* Unicode: U+%04X (%s)%s, Size: %dx%d" % [
                     code, code >= 32 ? code.chr(Encoding::UTF_8) : '?',
                     unidesc, g.width, g.height ]
            puts "   * Glyph (bounding: <x: %{x}, y: %{y}, " \
                                     "width: %{w}, height: %{h}>):" % bounding
            g.to_text.each {|line|
                puts "   *   #{line}"
            }
            puts "   * Encoded data: offset=#{offset}, size=#{bin.size}"
            puts "   */"
            bin.each_byte.map {|b| "0x%02x" % b }
                .each_slice(8).map {|row| row.join(', ') }
                .each {|row| puts "  #{row}," }
            puts "  /* <no data> */" if bin.size.zero?
            puts ""
            offset += bin.size
        }
        puts "};"
        puts ""
        
        # Glyph description
        puts "/* Store the glyph descriptions"
        puts " */"
        puts "static const lv_font_fmt_txt_glyph_dsc_t glyph_dsc[] = {"
        puts "  /* Reserved */"
        puts "  { 0 },"
        offset = 0
        list.each {|code| g = glyphs[code]
            bounding = g.bounding
            bin      = g.extract(bounding).to_bin
            puts "  /* Unicode: U+%04X (%s) */" % [
                     g.code, g.code >= 32 ? g.code.chr(Encoding::UTF_8) : '?' ]
            puts "  { .bitmap_index=%d, .adv_w=%d, .box_h=%d, .box_w=%d, .ofs_x=%d, .ofs_y=%d }," % [offset, (g.width * 16).to_i,
                                                                                                     bounding[:h], bounding[:w],
                                                                                                     bounding[:x], g.height - bounding[:y] - bounding[:h] - @base_line ]
            offset += bin.size
        }
        puts "};"
        puts ""

        # Sparse set
        cmaps.select {|cmap| Array === cmap }.each_with_index {|cmap, idx|
            puts "/* Unicode list for sparse set #{idx}:"
            cmap.map {|c| "U+%04X" % [ c ] }
                .each_slice(8).map {|g8| g8.join(', ') }
                .each {|l| puts " * #{l}," }
            puts " */"
            puts "static const uint16_t unicode_list_#{idx}[] = {"
            cmap.map {|c| "0x%04x" % [ c - cmap.first ] }
                .each_slice(8).map {|g8| g8.join(', ') }
                .each {|l| puts "    #{l}," }
            puts "};"
            puts ""
        }
        
        
        puts "/* Collect the unicode lists and glyph_id offsets"
        puts " */"
        puts "static const lv_font_fmt_txt_cmap_t cmaps[] = {"
        glyph_id_start  = 1
        unicode_list_id = 0
        cmaps.each {|cmap|
            case cmap
            when Range
                puts "    { .range_start       = #{cmap.first},"
                puts "      .range_length      = #{cmap.size},"
                puts "      .type              = LV_FONT_FMT_TXT_CMAP_FORMAT0_TINY,"
                puts "      .glyph_id_start    = #{glyph_id_start},"
                puts "    },"
            
            when Array
                puts "    { .range_start       = #{cmap.first},"
                puts "      .range_length      = #{cmap.last - cmap.first + 1},"
                puts "      .type              = LV_FONT_FMT_TXT_CMAP_SPARSE_TINY,"
                puts "      .glyph_id_start    = #{glyph_id_start},"
                puts "      .unicode_list      = unicode_list_#{unicode_list_id},"
                puts "      .list_length       = #{cmap.size},"
                puts "    },"
                unicode_list_id += 1
            end

            glyph_id_start += cmap.size
        }
        puts "};"
        
        puts ""
        puts "/* Store all the custom data of the font"
        puts " */"
        puts "static lv_font_fmt_txt_dsc_t font_dsc = {"
        puts "    .glyph_bitmap = glyph_bitmap,"
        puts "    .glyph_dsc    = glyph_dsc,"
        puts "    .cmaps        = cmaps,"
        puts "    .cmap_num     = #{cmaps.size},"
        puts "    .bpp          = #{bpp},"
        puts "};"


        puts ""
        puts "/* Initialize a public general font descriptor"
        puts " */"
        puts "lv_font_t lv_font_#{name} = {"
        puts "    .dsc               = &font_dsc,"
        puts "    .get_glyph_bitmap  = lv_font_get_bitmap_fmt_txt,"
        puts "    .get_glyph_dsc     = lv_font_get_glyph_dsc_fmt_txt,"
        puts "    .line_height       = #{line_height},"
        puts "    .base_line         = #{@base_line},"
        puts "};"

        puts ""
        puts "#endif"
    end
end




######################################################################


PROGNAME = $0.split('/')[-1]

RANGE_ALIAS = {
    'basic_latin'      => [ 0x0000, 0x007e ],
    'latin1_sup'       => [ 0x00a0, 0x00ff ],
    'cyrillic'         => [ 0x0410, 0x044f ],
    'arrow'            => [ 0x2190, 0x21ff ],
    'box_drawing'      => [ 0x2500, 0x256f ],
    'block_elements'   => [ 0x2580, 0x259f ],
    'geometric_shapes' => [ 0x25a0, 0x25ff ],
    'all'              => [ 0x0000, 0xffff ],
    'misc_symbols'     => [ 0x2600, 0x26ff ],
    'dingbats'         => [ 0x2700, 0x27bf ],    
    'muscial_symbols'  => [ 0x1d100, 0x1d1ff ],    
    'weather'          => [ 0x2600, 0x2601, 0x2614, 0x2602,
                            0x2603, 0x2604, 0x26c4, 0x26c5 ],
    'starts'           => [ 0x2605, 0x2606 ],
}


DEFAULT_FILTER = [0x0020..0x007e, 0x00a0..0x00ff]

#
# Command line parsing
#

$opts = Struct.new(:filter, :height, :name, :bpp, :base_line, :source)
              .new([],      nil,     nil,   1,    0         , nil    )


if ENV.include?('ZEPHYR_BASE')
    $opts.source = :zephyr
end

parser = OptionParser.new do |opts|
    opts.banner = "Usage: #{PROGNAME} [options] font.hex"

    opts.on("-r", "--range [RANGE]", "Range selection") do |r|
        if r = case r
               when /^(\d+)\.\.(\d+)$/     then [ $1.to_i,     $2.to_i     ]
               when /^0x(\d+)\.\.0x(\d+)$/ then [ $1.to_i(16), $2.to_i(16) ]
               when /^(.)-(.)$/            then [ $1.ord,      $2.ord      ]
               else RANGE_ALIAS[r] 
               end
            f, l = r.sort
            $opts.filter << (f .. l)
        else
            warn "Invalid range provided, can be:"
            warn "  - character range: a-z"
            warn "  - integer range     : 32..127"
            warn "  - hexadeciaml range : 0x0020..0x007e"
            warn "  - alias             : basic, latin_sup, cyrillic"
            exit
        end
    end
    opts.on("-s", "--set [STRING]",  "Set selection") do |s|
        $opts.filter += s.split('')
    end
    opts.on("--bpp [1,2,4,8]", Integer, "Bit-per-pixel") do |b|
        if [ 1, 2, 4, 8 ].include?(b)
            $opts.bpp = b
        else
            warn "Only BPP of 1, 2, 4 or 8 are supported"
            exit
        end
    end
    opts.on("--name [STRING]", "Font name") do |n|
        $opts.name = n
    end
    opts.on("--baseline [INTEGER]", Integer, "Font base line") do |b|
        $opts.base_line = b
    end
    opts.on("--height [INTEGER]", Integer, "Font height") do |h|
        $opts.height = h
    end
    opts.on("-h", "--help", "Prints this help") do
        puts opts
        exit
    end
end

parser.parse!

if ARGV.empty?
    puts parser
    exit
end


$file = ARGV[0]


if $opts.name.nil?
    $opts.name = $file.split('/')[-1].sub(/\.[^.]+/, '')
                                     .gsub(/[^a-z0-9_]/, '_')
end

if $opts.filter.empty?
    $opts.filter = DEFAULT_FILTER
end


if $opts.height.nil?
    warn "Original font height need to be specified"
    exit
end

if $opts.bpp != 1
    warn "For now only 1 BPP is supported"
    exit
end




$fc = FontConverter.new(base_line: $opts.base_line)
$fc.import(* FontConverter::Glyph.from_hexfile($file, height: $opts.height))
$fc.select(*$opts.filter)
$fc.build($opts.name, bpp: $opts.bpp, source: $opts.source)

