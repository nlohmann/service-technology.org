FILES = $(wildcard *)

all: bpel2owfn

bpel2owfn: $(FILES:.bpel=.lola)

%.lola: %.bpel
	bpel2owfn  --parameter=reduce --parameter=small  --mode=petrinet   --input=$<    --format=lola  --output=$@     
