#pragma once

#include <array>
#include <cstdint>
#include <map>
#include <optional>
#include <string>
#include <unordered_map>

#include "dinput.h"
#include "fmt/format.h"

namespace smgm::input {

enum class KeyState { kReleased, kPressed, kInvalid };

enum class POVState {
  kReleased,
  kUp,
  kUpRight,
  kRight,
  kDownRight,
  kDown,
  kDownLeft,
  kLeft,
  kUpLeft,
  kInvalid
};

using Key = std::uint8_t;

inline constexpr std::array<std::string_view, 256> kKeyNames{
    "",              // 0
    "ESCAPE",        // 1
    "1",             // 2
    "2",             // 3
    "3",             // 4
    "4",             // 5
    "5",             // 6
    "6",             // 7
    "7",             // 8
    "8",             // 9
    "9",             // 10
    "0",             // 11
    "-",             // 12
    "=",             // 13
    "BACK",          // 14
    "TAB",           // 15
    "Q",             // 16
    "W",             // 17
    "E",             // 18
    "R",             // 19
    "T",             // 20
    "Y",             // 21
    "U",             // 22
    "I",             // 23
    "O",             // 24
    "P",             // 25
    "[",             // 26
    "]",             // 27
    "RETURN",        // 28
    "LCTRL",         // 29
    "A",             // 30
    "S",             // 31
    "D",             // 32
    "F",             // 33
    "G",             // 34
    "H",             // 35
    "J",             // 36
    "K",             // 37
    "L",             // 38
    ";",             // 39
    "'",             // 40
    "~",             // 41
    "LSHIFT",        // 42
    "\\",            // 43
    "Z",             // 44
    "X",             // 45
    "C",             // 46
    "V",             // 47
    "B",             // 48
    "N",             // 49
    "M",             // 50
    ",",             // 51
    ".",             // 52
    "/",             // 53
    "RSHIFT",        // 54
    "NUM *",         // 55
    "LALT",          // 56
    "SPACE",         // 57
    "CAPS",          // 58
    "F1",            // 59
    "F2",            // 60
    "F3",            // 61
    "F4",            // 62
    "F5",            // 63
    "F6",            // 64
    "F7",            // 65
    "F8",            // 66
    "F9",            // 67
    "F10",           // 68
    "NUMLOCK",       // 69
    "SCROLL",        // 70
    "NUM 7",         // 71
    "NUM 8",         // 72
    "NUM 9",         // 73
    "NUM -",         // 74
    "NUM 4",         // 75
    "NUM 5",         // 76
    "NUM 6",         // 77
    "NUM +",         // 78
    "NUM 1",         // 79
    "NUM 2",         // 80
    "NUM 3",         // 81
    "NUM 0",         // 82
    "NUM .",         // 83
    "",              // 84
    "",              // 85
    "OEM_102",       // 86
    "F11",           // 87
    "F12",           // 88
    "",              // 89
    "",              // 90
    "",              // 91
    "",              // 92
    "",              // 93
    "",              // 94
    "",              // 95
    "",              // 96
    "",              // 97
    "",              // 98
    "",              // 99
    "F13",           // 100
    "F14",           // 101
    "F15",           // 102
    "",              // 103
    "",              // 104
    "",              // 105
    "",              // 106
    "",              // 107
    "",              // 108
    "",              // 109
    "",              // 110
    "",              // 111
    "KANA",          // 112
    "",              // 113
    "",              // 114
    "ABNT_C1",       // 115
    "",              // 116
    "",              // 117
    "",              // 118
    "",              // 119
    "",              // 120
    "CONVERT",       // 121
    "",              // 122
    "NOCONVERT",     // 123
    "",              // 124
    "YEN",           // 125
    "ABNT_C2",       // 126
    "",              // 127
    "",              // 128
    "",              // 129
    "",              // 130
    "",              // 131
    "",              // 132
    "",              // 133
    "",              // 134
    "",              // 135
    "",              // 136
    "",              // 137
    "",              // 138
    "",              // 139
    "",              // 140
    "NUM =",         // 141
    "",              // 142
    "",              // 143
    "PREVTRACK",     // 144
    "AT",            // 145
    "COLON",         // 146
    "UNDERLINE",     // 147
    "KANJI",         // 148
    "STOP",          // 149
    "AX",            // 150
    "UNLABELED",     // 151
    "",              // 152
    "NEXTTRACK",     // 153
    "",              // 154
    "",              // 155
    "NUM ENTER",     // 156
    "RCTRL",         // 157
    "",              // 158
    "",              // 159
    "MUTE",          // 160
    "CALCULATOR",    // 161
    "PLAYPAUSE",     // 162
    "",              // 163
    "MEDIASTOP",     // 164
    "",              // 165
    "",              // 166
    "",              // 167
    "",              // 168
    "",              // 169
    "",              // 170
    "",              // 171
    "",              // 172
    "",              // 173
    "VOLUMEDOWN",    // 174
    "",              // 175
    "VOLUMEUP",      // 176
    "",              // 177
    "WEBHOME",       // 178
    "NUMPADCOMMA",   // 179
    "",              // 180
    "NUM /",         // 181
    "",              // 182
    "PRTSC",         // 183
    "RALT",          // 184
    "",              // 185
    "",              // 186
    "",              // 187
    "",              // 188
    "",              // 189
    "",              // 190
    "",              // 191
    "",              // 192
    "",              // 193
    "",              // 194
    "",              // 195
    "",              // 196
    "PAUSE",         // 197
    "",              // 198
    "HOME",          // 199
    "UP",            // 200
    "PGUP",          // 201
    "",              // 202
    "LEFT",          // 203
    "",              // 204
    "RIGHT",         // 205
    "",              // 206
    "END",           // 207
    "DOWN",          // 208
    "PGDN",          // 209
    "INSERT",        // 210
    "DELETE",        // 211
    "",              // 212
    "",              // 213
    "",              // 214
    "",              // 215
    "",              // 216
    "",              // 217
    "",              // 218
    "LWIN",          // 219
    "RWIN",          // 220
    "APPS",          // 221
    "POWER",         // 222
    "SLEEP",         // 223
    "",              // 224
    "",              // 225
    "",              // 226
    "WAKE",          // 227
    "",              // 228
    "WEBSEARCH",     // 229
    "WEBFAVORITES",  // 230
    "WEBREFRESH",    // 231
    "WEBSTOP",       // 232
    "WEBFORWARD",    // 233
    "WEBBACK",       // 234
    "MYCOMPUTER",    // 235
    "MAIL",          // 236
    "MEDIASELECT",   // 237
    "",              // 238
    "",              // 239
    "",              // 240
    "",              // 241
    "",              // 242
    "",              // 243
    "",              // 244
    "",              // 245
    "",              // 246
    "",              // 247
    "",              // 248
    "",              // 249
    "",              // 250
    "",              // 251
    "",              // 252
    "",              // 253
    "",              // 254
    "",              // 255
};

enum class KeyNameFormat { kPreferNames, kNameless };

std::string GetKeyName(Key key,
                       KeyNameFormat format = KeyNameFormat::kPreferNames);

std::optional<Key> GetKeyFromName(const std::string& key_name);

int GetKeyPriority(input::Key key);

std::string_view StrGuidType(const GUID& guid_type);

}  // namespace smgm::input
