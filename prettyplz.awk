#!/usr/bin/awk -f
# Prettifies a JSON file. Use like:
#     ./prettyplz < myfile.json
# Author: silverweed
# License: WTFPL

function print_indent(ind,    i) {
	for (i = 0; i < ind; ++i)
		printf "\t"
}

{
	for (c = 1; c <= length($0); ++c) {
		ch = substr($0, c, 1)
		if (ch == "[" || ch == "{") {
			printf ch"\n"
			print_indent(++indent)
		} else if (ch == "(") {
			in_args = 1
			printf "("
		} else if (ch == ")") {
			in_args = 0
			printf ")"
		} else if (ch == ",") {
			if (in_args)
				printf ", "
			else {
				printf ",\n"
				print_indent(indent)
			}
		} else if (ch == "]" || ch == "}") {
			print ""
			print_indent(--indent)
			printf ch 
		} else if (ch == ":") {
			printf ": "
		} else {
			printf ch
		}
	}
	print ""
}
