# Flows

A `Source -> Flow -> Sink` style C++ library for passively processing many
inputs.

Conceptually, a `Flow` is a `Kleisli []` with `Monad` instance defined as
```
pure x = const [x]
join f = (f &&& id) >>> Kleisli (uncurry runKleisli)
```
Here I use the name `pure` to avoid conflicts with the `return` keyword in
C++, and the name `join` to avoid the reversed associativity of operator
`>>=`, also for a simpler implementation.

Beware that in C++ there is no guarantee of referential transparency; so
the scheme of `a -> [b]` is more like `a -> STT [] b`, which may produce
surprising (to some) results.  In particular, the `ap` operation, like
```
f <*> x = (f &&& x) >>> arr (uncurry ($))
```
may run the right side `x` more than one time for each input --- actually
it is run once for each output from `f`.

On the C++ side, a `Flow` does not actually return a list for each input;
instead it works by pushing each value to downstream.
