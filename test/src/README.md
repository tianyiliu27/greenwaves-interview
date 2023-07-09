
exo1and2 resume : 

	Description
	-----------

	1. (intern) Write a program which would make all the cores print
	   “hello world [cluster_id, core_id]”.
	2. (intern) Write a bash script named `launch.sh` that compiles and launches
	   your code.

	How to run the test : 
	-----------
	To run : 
	./launch.sh

	Additional Information : 
	-----------
	Inspired from the example HelloWorld 

	Expected result : 
	-----------

	     *** Task 1 ***

	hello world [32 0] 
	hello world [0 0] 
	hello world [0 5] 
	hello world [0 1] 
	hello world [0 4] 
	hello world [0 2] 
	hello world [0 6] 
	hello world [0 3] 
	hello world [0 7] 
	Test success !


exo3and4and5 resume:
	Description
	-----------

		3. (intern) Write a program which copy two square matrices (minimum size 64x64)
		   of type `unsigned short` from L2 memory to L1 memory.
		   Then produce the results of the two following operations by using the
		   cluster and send the result matrices from the L1 to L2:
		    - Addition of the 2 matrices
		    - Multiplication of the 2 matrices
		4. (new graduate) Extend the previous `launch.sh` script to accept a matrix
		   size parameter.
		   For example, entering `./launch.sh -s 128` will select a matrix size of
		   128x128. Use a default value of 64 when size is not specified.
		5. (new graduate) Using the result of the matrices multiplication above, do a
		   convolution of this matrix with the filter below:
			Try 2D dma copy : 
			- goal is to retrieve matrice at L1 memory at a 2D array so easy to do multiplication 
			- debug on address offset 

	How to run the test : 
	-----------
		To run : 
		./launch.sh

	Additional Information : 
	-----------
		Inspired from the example cluster_dma 

	Expected result : 
	-----------
			 *** PMSIS Cluster DMA Test ***

		Entering main controller
		Sending task.
		Cluster master core entry
		Core 0 requesting DMA transfer from l2_in to l1_buffer.
		Core 0 : Transfer done.

		************** Verification Transfer L2 to L1 ************** 
		 L2 to L1 success ! 

		Core 0 requesting DMA transfer from l1_buffer to l2_out.
		Core 0 : Transfer done.
		Cluster master core exit

		************** Verification Transfer L2 to L1 ************** 
		 L1 to L2 success ! 


		************** Verification convolution matrice  ************** 
		 Matrice Before Convolution 
		1 1 1 
		1 1 1 
		1 1 1 
		 Matrice After Convolution 
		0 0 0 
		28 182 234 
		227 23 106 
		End of convolution matrice operation 

		Close cluster after end of computation.


	BuG : 
		When I wanted like to copy from L2 to L1 the entire 2D array by using 2D dma transfer. However, it can only copy the first raw !

	I find another alternative  : 
		To do in another way :
		Using 1D DMA transfer from L2 to L1
		In L1 : Transform the List into 2DArray
		Do operation : addition + multiplication 
		In L1 : Transform 2D Array into List
		Using 1D DMA transfer from L1 to L2


To do :

 		4. (new graduate) Extend the previous `launch.sh` script to accept a matrix
	   size parameter.
	   For example, entering `./launch.sh -s 128` will select a matrix size of
	   128x128. Use a default value of 64 when size is not specified.

	   6. (experienced) Optimize all previous operations as efficiently as possible.
