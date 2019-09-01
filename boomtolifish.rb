#!/usr/bin/ruby
# Convert BOOM json levels to lifish levels

require 'json'

unless ARGV.length > 0
	puts "Usage: #{$0} <boom_levels.json>"
	exit 1
end

file = File.read ARGV[0]
boom_levels = JSON.parse(file)['LevelDescription']

# A BOOM level's structure is:
# {
#	"FixedBlockID": num,
#	"BreakableBlockID": num,
#	"BorderID": num,
#	"BGPatternID": num,
#	"GridDescString: string,
#	"Time": num
# }
#
# We need to convert it to lifish format:
# {
#	"time": num,
#	"music": num,
#	"tileIDs": {
#		"bg": num,
#		"border": num,
#		"fixed": num,
#		"breakable": num
#	},
#	"tilemap": string
# }

lifish_hash = {
	"name" => "Converted BOOM levels",
	"author" => "Federico Filipponi",
	"difficulty" => "average",
	"tracks" => [
		{
			"name" => "Go For It",
			"author" => "George E. Kouba, Jr.",
			"loop" => {
				"start" => 0.0,
				"length" => 0.0
			}
		},
		{
			"name" => "What Goes Around",
			"author" => "George E. Kouba, Jr.",
			"loop" => {
				"start" => 0.0,
				"length" => 0.0
			}
		},
		{
			"name" => "Bomber Boy",
			"author" => "George E. Kouba, Jr.",
			"loop" => {
				"start" => 0.0,
				"length" => 0.0
			}
		},
		{
			"name" => "Smoak Et",
			"author" => "George E. Kouba, Jr.",
			"loop" => {
				"start" => 0.0,
				"length" => 0.0
			}
		},
		{
			"name" => "Terminate",
			"author" => "George E. Kouba, Jr.",
			"loop" => {
				"start" => 0.0,
				"length" => 0.0
			}
		},
		{
			"name" => "Fused",
			"author" => "George E. Kouba, Jr.",
			"loop" => {
				"start" => 0.0,
				"length" => 0.0
			}
		},
		{
			"name" => "BoomRunner",
			"author" => "George E. Kouba, Jr.",
			"loop" => {
				"start" => 0.0,
				"length" => 0.0
			}
		},
		{
			"name" => "Boom Forever",
			"author" => "George E. Kouba, Jr.",
			"loop" => {
				"start" => 0.0,
				"length" => 0.0
			}
		}
	],
	"enemies" => [
		{
			"name" => "soldier",
			"ai" => 0,
			"speed" => 1.0,
			"attack" => {
				"type" => [
					"simple",
					"blocking"
				],
				"id" => 1,
				"fireRate" => 1.0,
				"blockTime" => 180.0
			}
		},
		{
			"name" => "sgt. cool",
			"ai" => 0,
			"speed" => 1.0,
			"attack" => {
				"type" => [
					"simple",
					"blocking"
				],
				"id" => 1,
				"fireRate" => 1.5,
				"blockTime" => 180.0
			}
		},
		{
			"name" => "thick lizzy",
			"ai" => 1,
			"speed" => 1.0,
			"attack" => {
				"type" => [
					"simple",
					"blocking"
				],
				"id" => 2,
				"fireRate" => 1.0,
				"blockTime" => 200.0
			}
		},
		{
			"name" => "mean-o-taur",
			"ai" => 2,
			"speed" => 2.0,
			"attack" => {
				"type" => [
					"contact"
				],
				"contactDamage" => 2,
				"fireRate" => 5.0
			}
		},
		{
			"name" => "gunner",
			"ai" => 1,
			"speed" => 1.0,
			"attack" => {
				"type" => [
					"blocking"
				],
				"id" => 3,
				"fireRate" => 7.0,
				"blockTime" => 200.0
			}
		},
		{
			"name" => "thing",
			"ai" => 3,
			"speed" => 1.0,
			"attack" => {
				"type" => [
					"simple",
					"blocking"
				],
				"id" => 4,
				"fireRate" => 1.0,
				"blockTime" => 250.0
			}
		},
		{
			"name" => "ghost",
			"ai" => 4,
			"speed" => 1.0,
			"attack" => {
				"type" => [
					"contact",
					"ranged"
				],
				"fireRate" => 2.0
			}
		},
		{
			"name" => "smoulder",
			"ai" => 3,
			"speed" => 1.0,
			"attack" => {
				"type" => [
					"blocking",
					"ranged"
				],
				"id" => 5,
				"fireRate" => 4.0,
				"blockTime" => 260.0,
				"tileRange" => 4
			}
		},
		{
			"name" => "skully",
			"ai" => 3,
			"speed" => 1.0,
			"attack" => {
				"type" => [
					"blocking"
				],
				"id" => 6,
				"fireRate" => 6.0,
				"blockTime" => 200.0
			}
		},
		{
			"name" => "h.r. giggler",
			"ai" => 3,
			"speed" => 2.0,
			"attack" => {
				"type" => [
					"simple",
					"blocking"
				],
				"id" => 7,
				"fireRate" => 0.7,
				"blockTime" => 650.0
			}
		}
	],
	"levels" => []
}

boom_levels.each_with_index do |level, i|
	lv = {
		"num" => i + 1,
		"time" => level["Time"],
		"music" => (i / 10).floor + 1,
		"width" => 15,
		"height" => 13,
		"effects" => [],
		"tileIDs" => {
			"bg" => level["BGPatternID"],
			"border" => level["BorderID"] + 1,
			"fixed" => level["FixedBlockID"] + 1,
			"breakable" => level["BreakableBlockID"] / 4 + 1
		},
		"tilemap" => level["GridDescString"]
	}
	lifish_hash["levels"] << lv
end

File.open("converted.json", "w") do |f|
	f.write lifish_hash.to_json
end
