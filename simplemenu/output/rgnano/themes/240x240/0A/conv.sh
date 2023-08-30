#!/bin/bash

for org in `find . -name 'logo.png'`; do
	convert $org -resize 240x240\! $org
done

