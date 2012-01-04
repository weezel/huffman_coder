# Huffman ASCII decoder

### Info
Deciphers `example.txt` ASCII tree to clear text. Can eat arbitrary file via
(command line) parameter. Based on Huffman encoding.

Utterly stupid stupid program that I made at some point. Has plenty of glue
that should be amended. Maybe some day I will be enough furious that I fix 'em.

Sample run against `example.txt`:

	        X           
	       / \          
	      X   X         
	     / \   \        
	    X  [w]  X       
	   / \     / \      
	 [z]  X  [l]  X     
	     / \     / \    
	   [@] [f]  X  [e]  
	           / \      
	          X   X     
	         / \   \    
	       [i] [c]  X   
	               / \  
	              X  [s]
	             / \    
	           [k] [.]  

Code is: 0111111111000111111000100011111011111100111101100111011010011111000
Deciphered code: weezel@XXXX.XX (domain superseded)


### Future plans
* Maybe some day I am enough wacko that I'll implement functionality that generates a
random tree from the given binary code input.

* Finalize `tests.c`.

* Fix things here and there.

