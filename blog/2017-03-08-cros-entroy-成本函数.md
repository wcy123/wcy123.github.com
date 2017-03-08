---
title: 成本函数与学习速度
---

[back propagation](2017-03-05-backpropagation.html) 里提到模型，
我们十分关心 $\frac{\partial{C}}{\partial{w_{l-1,j,i}}}$ 和 $\frac{\partial{C}}{\partial{b_{l,j}}}$ ，因为这个是学习速度，或者叫做训练速度。这个值很大的话，就会很快收敛。训练一个神经网路需要很大的计算量，如果能提高  $\frac{\partial{C}}{\partial{w_{l-1,j,i}}}$  和 $\frac{\partial{C}}{\partial{b_{l,j}}}$，那么我们就可以很大提升训练效率。他们的含义是指 ${w_{l-1,j,i}}$ 变化一点点，导致 $C$ 变化是不是很大。我们希望越大越好。因为梯度求解过程中，我们知道 $C$ 一定会变小，奔向最低点，那么何不让他跑的快一点。

# 成本函数

成本函数需要有几个特点

1. 非负。 $C>0$
2. 收敛性。当输出越接近目标值的时候，$C$ 越小。当输出等于目标值的时候，$C$ 为零。

一个简单的成本函数是通常平方和函数。

$$C = \frac{1}{2} \sum_{k=0}^{N_L - 1} (y_{k}-a_{L-1,k})^2$$

[back propagation](2017-03-05-backpropagation.html) 中，我们看到下面的公式

$$
\begin{align}
\frac{\partial{C}}{\partial{w_{l-1,j,i}}}
   &= \delta_{l,j} a_{l-1,i} \\
\frac{\partial{C}}{\partial{b_{l,j}}} &=  \delta_{l,j} \\
\end{align}
$$

其中

$$
\begin{align}
\delta_{L-1,j} &=
\frac{\partial{C}}{\partial{a_{L-1,j}}} \sigma'({z_{L-1,j}}) \\
\delta_{l-1,j} &= \sum_{i=0}^{N_{l} -1}
    \delta_{l,i} w_{l-1,j,i} \sigma'(z_{l-1,j})
\end{align}
$$



这里看到 这两个值主要的大小和 $\frac{\partial{C}}{\partial{a_{L-1,j}}}$ 和 $\sigma'(x)$ 成正比例关系。

对于平方和函数来说

$$ \frac{\partial{C}}{\partial{a_{L-1,j}}} = (a_{L-1,j} - y_j) $$

当我们猜测了一个权重值和偏移量，导致神经元产生了很大的 $z_{L-1,j}$，如果选定 sigmoid 函数 $\sigma(x)=\frac{1}{1+e^{-x}}$ 的时候

$$\sigma'(x) = \sigma(x) (1- \sigma(x))$$

可以 $x$ 趋近于 $+\infty$ 或者 $-\infty$ 的时候， $\sigma(x)$ 趋近于 $0$ 或者 $1$ 。

如果不改变神经元函数的前提下，也就是说，无论如何， $\sigma'(x)$ 趋近于都 $0$。这个时候，学习速度十分缓慢。如何提高学习速度？

重复提一遍，学习速度是指 $\frac{\partial{C}}{\partial{w_{l-1,j,i}}}$ 和 $\frac{\partial{C}}{\partial{b_{l,j}}}$ 这两个值主要的大小和 $\frac{\partial{C}}{\partial{a_{L-1,j}}}$ 和 $\sigma'(x)$ 成正比例关系。

既然 $\sigma'(x)$ 已经很小了，接近于 $0$ ，那么我们需要改变成本函数 $C$，提高
$\frac{\partial{C}}{\partial{a_{L-1,j}}}$

如果选择另一个 cross entroy 成本函数
（TODO: 下面的公式似乎不够清晰）
$$
C = - \frac{1}{n} \sum_{x} (y\log a + (1-y) \log(1-a)))
$$

这样
$$
\begin{align}
 \frac{\partial{C}}{\partial{a_{L-1,j}}}
 &= -\frac{1}{n} \left(
   y_{j} \frac{1}{a_{L-1,j}} +
   (1- y_j) \frac{1}{1-a_{L-1,j}} \cdot -1
   \right)
 \\
 &= -\frac{1}{n} \left(
   \frac{y_j}{a_{L-1,j}} +
   \frac{y_j - 1}{1-a_{L-1,j}}
   \right) \\
 &= -\frac{1}{n}
     \frac{y_j - y_j a_{L-1,j} + a_{L-1,j} y_j - a_{L-1,j}}
     {a_{L-1,j}(1-a_{L-1,j})}\\
 &= \frac{1}{n}
          \frac{a_{L-1,j} - y_j }
          {a_{L-1,j}(1-a_{L-1,j})}
   \\
\end{align}
$$

(TODO 检查上面的推导，是不是最后 ${a_{L-1,j}(1-a_{L-1,j})}$ 可以约掉了？因为 $\sigma'(x) = \sigma(x) (1- \sigma(x))$)

可以看到，这样的成本函数，在 $a_{L-1,j}$ 趋近于 $1$ 或者 $0$ 的时候， 都会导致 $\frac{\partial{C}}{\partial{w_{l-1,j,i}}}$ 变得很大。
