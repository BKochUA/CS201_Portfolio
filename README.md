# CS201_Portfolio Brian Koch - 11653007
I tried with this project, I really did. I've put hundreds of hours in but I just could not get it working.
I started by attempting to load the food dataset into memory with a Trie tree. I spent a lot of time getting it to work but then when I scaled it up to the full dataset it was much too space intensive and froze up.
I then adapted to a Radix tree structure, which worked extremely well. I was able to load it into memory extremely fast and searching was going well. 
but then I realized that the searching was not returning all items that were in the dataset, only some. This started the process of working on the function "fillOutTree". The goal of this is to traverse every node and build the names from it. In theory this is easy, I based it on the recursive code for preorder traversal of a binary tree, with radixnode->link being left and radixnode->next being right. unfortunately, despite spending lots of time on it, I could not get it working. It was better than my other search method in that it would reach many more items but it would build some incorrectly still.

After realizing that I didn't have much time left I decided to scrap that and go back to the old searching that didn't return every item but at least didn't give errors. Unfortunately I didn't have time then to implement the diary very well, and here we are, with an unfinished project. As you can see, I was able to efficiently implement a data structure, but because I wasted so much time on that, I was unable to finish the actual diary aspect.

if you want to test the project, I would suggest the search screen, it works pretty well. You can type the beginning of a product name, results will show up, and you can use the arrow keys and enter to select an item and view it's details.

as for borrowed code, I've marked in my comments where I have borrowed from.
