pandoc -N --variable "geometry=margin=1in" --variable mainfont="Times" --variable sansfont="Times" --variable monofont="Monaco" --variable fontsize=12pt --variable version=2.0 -i user_manual.md --include-in-header user_manual_latex_header.tex --pdf-engine=xelatex --toc -o User_Manual_$1.pdf   
