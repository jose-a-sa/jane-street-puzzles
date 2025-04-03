# Sum One, Somewhere

![](sum-one-somewhere.png)

For a fixed p, independently label the nodes of an infinite complete binary tree 0 with probability p, and 1 otherwise. For what p is there exactly a 1/2 probability that there exists an infinite path down the tree that sums to at most 1 (that is, all nodes visited, with the possible exception of one, will be labeled 0). Find this value of p accurate to 10 decimal places.

## Solution

Given an infinite complete tree, we can denote by $r = P[X]$ is the probability of a tree having an infinite path that sums to at most 1. Given the self-similarity in this case, $r = P[X]$ will also apply to denote the probability for any subtree.

We can divide path configurations into 2 case: either the path consists of all zeros or contains a single 1 and all zeros. So if we encounter a subtree with a root 0 (with probability $p$), the probability of having an infinite path that sums to at most 1 is equivalent to either the left OR right subtrees having such a path:
$$P[X^L \cup X^R] = P[X^L] + P[X^R] - P[X^L \cap X^R] = 2r - r^2$$
On the other hand, if the root node has value 1 (with probability $1-p$), then the probability is given by the probability that any of the left/right subtrees contains a path that sums to exactly 0.
$$P[Y^L \cup Y^R] = P[Y^L] + P[Y^R] - P[Y^L \cap Y^R] = 2q - q^2$$
where $P[Y^L] = P[Y^r] = q$ is the probability of a subtree containing a path with all 0s.
Combining the result we have:
$$ r = p (2r - r^2) + (1-p) (2q-q^2)  \tag{1}$$
Futhermore, in other to have a path with all zeros the root must be 0 and any of the subtrees must have a path with all 0s.
$$ q = p (2q - q^2) \tag{2} $$

Replacing the required $r = 1/2$, multiplying the (1) equation by $4(2q-q^2)$ and replacing by (2), we get
$$ 2(2q-q^2) = 3q + 4(2q-q^2)(2q-q^2 - q) \implies  q (4 q^3 - 12 q^2 + 10 q  -1) = 0$$
Assuming $p,q\in(0,1)$, we have $p q = 2p - 1$.
A bit more algebraic manipulation and we arrive to
$$3 p^3 - 10 p^2 + 12 p - 4 = 0$$
$$4 q^3 - 12 q^2 + 10 q  -1 = 0$$
These equations have a unique real solutions at
$$p = 0.530603575430005\ldots$$
$$q = 0.115353822880684\ldots$$
