# ParallelAndDistributedProgramming

[Lab 1 - Non-cooperative multithreading](Supermarket_inventory_lab1)  
Supermarket inventory.
* There are several types of products, each having a known, constant, unit price. In the begining, we know the quantity of each product.
* We must keep track of the quantity of each product, the amount of money (initially zero), and the list of bills, corresponding to sales. Each bill is a list of items and quantities sold in a single operation, and their total price.
* We have sale operations running concurrently, on several threads. Each sale decreases the amounts of available products (corresponding to the sold items), increases the amount of money, and adds a bill to a record of all sales.
* From time to time, as well as at the end, an inventory check operation shall be run. It shall check that all the sold products and all the money are justified by the recorded bills.

[Lab 2 - Producer-consumer synchronization](Producer_consumer_lab2)  
Compute the scalar product of two vectors.
* Create two threads. The first thread (producer) will compute the products of pairs of elements - one from each vector - and will feed the second thread. The second thread (consumer) will sum up the products computed by the first one. The two threads will behind synchronized with a condition variable and a mutex. The consumer will be cleared to use each product as soon as it is computed by the producer thread.

[Lab 3 - Simple parallel tasks](Product_matrices_lab3)  
Compute the product of two matrices.

* Have a function that computes a single element of the resulting matrix.
* Have a second function whose each call will constitute a parallel task (that is, this function will be called on several threads in parallel). This function will call the above one several times consecutively to compute several elements of the resulting matrix.
* Each task computes consecutive elements, going row after row. So, task 0 computes rows 0 and 1, plus elements 0-1 of row 2 (20 elements in total); task 1 computes the remainder of row 2, row 3, and elements 0-3 of row 4 (20 elements); task 2 computes the remainder of row 4, row 5, and elements 0-5 of row 6 (20 elements); finally, task 3 computes the remaining elements (21 elements).

* For running the tasks, implement 2 approaches:
    * Create an actual thread for each task (use the low-level thread mechanism from the programming language);
    * Use a thread pool.

[Lab 4 - Futures and continuations C#](Futures_continuations_lab4)  
Write a program that is capable of simultaneously downloading several files through HTTP. Use directly the BeginConnect()/EndConnect(), BeginSend()/EndSend() and BeginReceive()/EndReceive() Socket functions, and write a simple parser for the HTTP protocol (it should be able only to get the header lines and to understand the Content-lenght: header line).

Try three implementations:
  * Directly implement the parser on the callbacks (event-driven);
  * Wrap the connect/send/receive operations in tasks, with the callback setting the result of the task;
  * Like the previous, but also use the async/await mechanism.
