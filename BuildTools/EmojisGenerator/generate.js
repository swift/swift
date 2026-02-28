var util = require("util"),
    fs   = require("fs"),
    _    = require("underscore");

// Load emojis
var emojis = require("./emoji.json");

var unicodeCodepointToByteArray = function (codepoint) {
    var utf8 = unescape(encodeURIComponent(String.fromCodePoint(parseInt(codepoint, 16))));

    var arr = [];
    for (var i = 0; i < utf8.length; i++) {
        arr.push(utf8.charCodeAt(i));
    }
    return arr;
}

var byteArrayToCStringLiteral = function (byteArray) {
    var literalString = "";
    for (var i = 0; i < byteArray.length; i++) {
        literalString += "\\x" + byteArray[i].toString(16);
    }
    return literalString;
}

var mapping = '';
// Generate C++ mapping for shortnameUnicode_
mapping += 'const std::unordered_map<std::string, std::string> EmojiMapper::shortnameUnicode = std::unordered_map<std::string, std::string>{';
mapping += _(emojis).filter(function(data) {
    // Filter out regional category.
    return data.category != 'regional';
}).filter(function(data) {
    // Only use emojis with 2 or less codepoints, as Qt's harfbuzz version
    // has issues rendering those as a single glyph.
    return data.unicode.split("-").length < 3;
}).map(function(data) {
    var shortname = data.shortname;
    // Get codepoints
    var codepoints = _(data.unicode.split("-")).map(function (code) {
        //return "\\U" + "0".repeat(8-code.length) + code;
        return byteArrayToCStringLiteral(unicodeCodepointToByteArray(code));
    });

    // .join('\\U0000200D') -> join characters with a zero width joiner, required by
    // some emojis pattern (:family_mwg: for example)
    // Currently do no join them by a ZWJ as it breaks rendering of a lot simple emojis
    // like $EMOJI followed by skin tone modifier.
    //return '{"' + shortname + '", "' + codepoints.join(byteArrayToCStringLiteral(unicodeCodepointToByteArray('200D'))) + '"}';
    return '{"' + shortname + '", "' + codepoints.join("") + '"}';
}).join(", ") + '};\n\n';

// Generate C++ code for the reverse mapping (i.e. unicodeShortname_ )
mapping += '    const std::unordered_map<std::string, std::string> EmojiMapper::unicodeShortname = [](){\n' +
           '        std::unordered_map<std::string, std::string> unicodeSequenceToShortname;\n' +
           '        const auto& shortnameToUnicodeMap = EmojiMapper::shortnameUnicode;\n' +
           '        for (const auto& shortnameToUnicode : shortnameToUnicodeMap) {\n' +
           '            unicodeSequenceToShortname[shortnameToUnicode.second] = shortnameToUnicode.first;\n' +
           '        }\n' +
           '        return unicodeSequenceToShortname;\n' +
           '    }();\n\n';

// Generate C++ mapping for categories
var CategoryMapping = new Map();
_(emojis).filter(function(data) {
    // Filter out regional category.
    return data.category != 'regional';
}).filter(function(data) {
    // Only use emojis with 2 or less codepoints, as Qt's harfbuzz version
    // has issues rendering those as a single glyph.
    return data.unicode.split("-").length < 3;
}).map(function(data, category) {
    // Get codepoints
    var codepoints = _(data.unicode.split("-")).map(function (code) {
        //return "\\U" + "0".repeat(8-code.length) + code;
        return byteArrayToCStringLiteral(unicodeCodepointToByteArray(code));
    });
    if (!CategoryMapping.has(data.category)) {
        CategoryMapping = CategoryMapping.set(data.category, []);
    }
    //CategoryMapping.get(data.category).push(codepoints.join(byteArrayToCStringLiteral(unicodeCodepointToByteArray('200D'))));
    // Currently do no join them by a ZWJ as it breaks rendering of a lot simple emojis
    // like $EMOJI followed by skin tone modifier.
    CategoryMapping.get(data.category).push(codepoints.join(""));
    //return 'categories_["' + data.category + '"].push_back("' + codepoints.join('\\U0000200D') + '");';
});

mapping += '    const std::unordered_map<std::string, std::vector<std::string>> EmojiMapper::emojisInCategory = std::unordered_map<std::string, std::vector<std::string>>{';
categoryMappings = [];
for (var category of CategoryMapping.keys()) {
    categoryMappings.push('{"' + category + '", {' + CategoryMapping.get(category).map(function (literal) {
        return '"' + literal + '"';
    }).join(', ') + '}}')
}
mapping += categoryMappings.join(', ');
mapping += '};\n'

mapping.replace(/^\s*\n/gm, '');

// Generate C++ class from template
var input  = fs.readFileSync("./EmojiMapper.cpp");
var output = _(input.toString()).template()({ mapping: mapping });

// Write C++ class to file
var output_path = "../../SwifTools/EmojiMapper.cpp";
fs.writeFileSync(output_path, output);

console.log("Generated " + output_path);
