# ACC_HC4 Method
## 中文版
加速GATK Haplotype Caller 的优化方式： 
ACC_HC4是基于FGPA的异构计算和多线程方式进行优化得;
GATK Haplotype Caller 最耗时计算是Pairhmm，主要用于计算read和Haplotype 的似然估计；Pairhmm的计算复杂度O（mn), m为read长度，n为haplotype的长度; 采用Intel开发Pairhmm模块进行优化。Pairhmm计算方式，采用服务器和客服端多个进程方式进行异步计算，进程间是通过共享内存方式进行通信，具有稳定高效的特性; 服务器主要接受客户端的数据，采用FPGA计算Pairhmm，而客户端可能有多个线程，每一个县城可能提供计算数据和等待计算结果。

ACC_HC, 采用多线程计算，来提高性能，具体实现方式如下：1）进行预处理，根据Reference信息，进行高效数据划分，划分等分N个区间，N为CPU数量的1/4左右; 如果是EXON数据，根据reference 和 exon region进行数据划分，划分成N个计算区间; 希望这N个计算区间能够达到较好的负载均衡; 每个计算区间，使用单生产者多消费者的多线程模式进行计算，其中线程比1：3或者1：4; 生产者线程主要处理Define active region， 消费者线程处理计算haplotype, likelihood, assign sample genotypes, output vcf/gvcf;最后，合并各个计算区间的结果输出最终结果;

## English

Accelerate GATK Haplotype Caller method
Optimize performance of GATK HC with mutithreading and heterogeneous compute; pairhmm algorithm determines likelihood of the haplotypes given the read data, simultaneously,high computational complexity; Accelerate GATK Haplotype Caller, using pairhmm compute module developed by Intel; Use server  and  client method to compute pairhmm. The client process and server process inter-process communication through shared memory mode, the client process, could have many threads, every thread could produce pairhmm data and wait for pairhmm result. The server process could accept pairhmm data and compute pairhmm using fpga. 

Meanwhile, Accelerate GATK Haplotype Caller using multi thread mode to improve performace; Pre-process, data partition mode base on the reference infomation, if exon mode, base on the reference infomation and exon region. It could divide as much as one quarter of the total cpu cores intervals. The Intervals has same size to load balance as much as possible. The Interval could using single produce multi consumer mode multi-threads to process. The produce thread process define active regions,  the consumer threads could have 3 or 4 thread to process, to determine haplotypes by re-assembly of the active region, determine likelihood of haplotypes given the read data using pairhm algorithm, assign sample genotypes and output temporary file. Finally, merge the temporary file to output vcf/ gvcf file. 
