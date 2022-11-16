# exactcover.lua

## Knuth's solvers

Some solvers from [Knuth's program page](https://cs.stanford.edu/~knuth/programs.html):

|solver|doc|comment|
|---|---|---|
|[dlx1.c](https://github.com/massimo-nocentini/exactcover.lua/blob/master/knuth/src/dlx1/dlx1.c)|[pdf](https://github.com/massimo-nocentini/exactcover.lua/blob/master/knuth/src/dlx1/dlx1.pdf),[cweb](https://cs.stanford.edu/~knuth/programs/dlx1.w)|Algorithm 7.2.2.1X for exact cover via dancing links|
|[dlx2.c](https://github.com/massimo-nocentini/exactcover.lua/blob/master/knuth/src/dlx2/dlx2.c)|[pdf](https://github.com/massimo-nocentini/exactcover.lua/blob/master/knuth/src/dlx2/dlx2.pdf),[cweb](https://cs.stanford.edu/~knuth/programs/dlx2.w)|Algorithm 7.2.2.1C, the extension to color-controlled covers|
|[dlx3.c](https://github.com/massimo-nocentini/exactcover.lua/blob/master/knuth/src/dlx3/dlx3.c)|[pdf](https://github.com/massimo-nocentini/exactcover.lua/blob/master/knuth/src/dlx3/dlx3.pdf),[cweb](https://cs.stanford.edu/~knuth/programs/dlx3.w)|Algorithm 7.2.2.1M, the extension to general column sums |


## Dependencies

Extract http://ftp.cs.stanford.edu/pub/sgb/sgb.tar.gz and then use the `Makefile`, so type `make tests` and then type `sudo make install`.
