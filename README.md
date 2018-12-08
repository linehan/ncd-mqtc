<img src="image/ytree.svg" align="right"></img>

# ncd-mqtc
Toolbench for working with Normalized Compression Distance (NCD) and the Minimum Quartet Tree Cost (MQTC) clustering algorithm.

## How do I use it?
Assemble a corpus in a directory, and use the `ncd` tool to generate an NCD datafile containing a similarity matrix for the corpus and a key to make it easier to tag the matrix elements with their filenames during subsequent processing.

Once you have the NCD data file, input it to the `mqtc` tool, along with a number of generations, and it will perform MQTC clustering and output its answer.

Note that MQTC is a Markov-chain Monte Carlo genetic algorithm.

## Which compressor to use?
The NCD is a family of similarity metrics, so you must specify a suitable data compressor to use. Each has benefits and tradeoffs. For a full explanation, see <paper link>.

## Walkthrough

Make everything:

        make all

Make just the `compress` program:

        make compress 

Make just the `cluster` program:

        make cluster


Instructions for `compress`:

        Run `./ncd` for usage.

Instructions for `cluster`:

        ./mqtc < 1000 _data/10x10.txt
        
