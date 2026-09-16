# RooFit hands-on course: one toy, three ways to fit it

A 1-hour guided session built around a single toy experiment shared by all
three parts. Every fit runs on the **same data** (`data.root`), produced by
one weighted MC simulator (`physics.h` + `make_toys.cpp`).

The toy physics:

| component | eta | phi | pt | mass |
|---|---|---|---|---|
| signal (Ns = 2000 at µ = 1) | Gauss(0, 0.5) | 1 + 0.4·cos(2φ) | Γ(k=2, 50 GeV) | **Gauss(125, 2)** |
| background (Nb = 20000) | exp(−θ·\|η\|), θ nuisance | flat | exp(−pt/20) | exp(−m/25) |

The MC is *weighted*: events are drawn once from a fixed proposal
distribution, and the physics parameters µ (signal strength) and θ (background
η slope) enter **only through per-event weights**, so we only need to reweight the sample, never regenerate.
This is very common for realistic anlyses, where nuisance parameters are often treated as MC weight variations (like in this toy example), or per-object scale factors for reconstruction uncertainties, e.g. for electron reconstruction efficiency.
The "real data" are drawn from the "truth"
(µ_true = 1, θ_true = 1.3, mass peak at 125 GeV).

## Setup

```
./run.sh          # everything, or step by step:
root -b -q make_toys.cpp   # simulator NTuples: sim.root, data.root
root -b -q unbinned.cpp    # Part 1: unbinned analytic-shape fit
root -b -q analysis.cpp    # Part 2a: RDF analysis + Vary templates
root -b -q histfactory.cpp # Part 2b: HistFactory fit
root -b -q likelihood.cpp  # Part 3: likelihood anatomy
```

File tour: `physics.h` (the "theory": densities, samplers, weight functions,
all parameters), `make_toys.cpp` (NTuple producer), then one macro per part.

## Part 1: Analytic shapes and unbinned fit

If we *know* the functional forms (signal a Gaussian, background an
exponential) we don't need templates at all. RDataFrame applies the
selection (pt > 20, |η| < 2.2), the surviving masses go into a
`RooDataSet`, and an extended maximum-likelihood fit
`model = nsig·Gauss(m) + nbkg·Exp(m)` measures the yields with the peak
position and background slope floating.

This is implemented in `unbinned.cpp`. Please run that code and answer the following questions:

- Complete the fit macro: the background is still missing in the model. Please add a RooFit pdf that corresponds to the exponential background with the `slope` parameter, and add it as a component for the final RooAddPdf
- Extended ML: the yields *are* fit parameters! The Poisson term for the
  total count is part of the likelihood. Where is the information on nsig
  actually coming from?
- Check the summary printout: nsig = ? vs. the expectation computed from
  `physics.h`. What consistency do you expect?
- The mass resolution (peak width) was fixed "from a resolution study".
  But does it have to be like that? Float it and watch what happens to σ(nsig) and to the
  correlations (`res->correlationMatrix()`). How do you think these resolution nuisance parameters are dealt with in real analyses?

## Part 2: Binned template fit (`make_toys.cpp`, `analysis.cpp`, `histfactory.cpp`)

### The motivating question

> **What if the background had no tractable analytic shape?**
> Detector effects, higher-order QCD, an MC-only truth... You can't write
> b(m; θ) down. But you *can* simulate events and reweight them. That's what
> out toy simulator can already do, so here's what we do: histogram the simulation (templates),
> and build a likelihood over binned Poisson counts. That's HistFactory.

### Intractable shapes: templates with HistFactory

Here is what we do: instead of fitting the `nsig` parameter as before, we'll with the so called "signal strength" parameter µ that is the ratio between `nsig` and the expected number of signal events from the simulator model.

The other parameters like `mean` ,`slope`, or `nbkg` are not needed this time, because the expected shapes are fixed by the simulator. However, the simulator is not perfect: it has tune parameters θ that are now our new nuisance parameters.

`make_toys.cpp` → `analysis.cpp` → `histfactory.cpp`

1. `make_toys.cpp`: the weighted simulator. Trace where µ and θ appear
   (weights only). Check `sum(sig_w)`/`sum(bkg_w)` against expectation and
   discuss what sets their uncertainty.
2. `analysis.cpp`: RDataFrame analysis in |η| — central signal bump vs.
   θ-tilted background slope. **Vary** produces the up/down templates for
   both parameters in a *single* event loop, from weights recomputed on the
   fly.
3. `histfactory.cpp`: programmatic HistFactory: signal template × NormFactor `mu`
   (POI), background template with a `HistoSys` built from the Vary
   histograms, MC statistical errors activated. Fit; read off
   θ̂ = θ_nom + α_θ·Δθ. Against true 1.3: does it close?

Discussion points:

- Why does θ live on the *background η shape* and not on the mass? (Deliberate
  course design: mass is "tractable", η is "intractable". Real analyses mix
  both in one model.)
- Where is the constraint on `alpha_theta` coming from if the data
  constrain it better than the ±1 templates? What does "α ≈ 0.77" mean
  physically?
- The `_channel_1 ... already in this set` ERROR line is from HistFactory's
  own workspace factory — harmless; good example of distinguishing signal
  from noise in tool output.

## Part 3: Understanding the profile likelihood

`likelihood.cpp` (loads `results/meas_combined_meas_model.root`)

Same fit result as Part 2, now dissected. The macro draws, for µ and
α_θ, the ΔNLL *slice* (other parameters frozen) and the ΔNLL *profile*
(others re-minimized at each point), plus the 2D (µ, α_θ) contours:

- The contour plot has x and y axis ranges that are too large to be appropriate
  for this plot. Can you improve this using `TAxis::SetRangeUser()`?
- The red profile curve crosses ΔNLL = 0.5 exactly at ±σ(HESSE) — *if* Wilks'
  theorem applies and the NLL is parabolic. Does it here? (Hint: look at
  α_θ's curve asymmetry.)
- corr(µ, α_θ) ≈ −0.42: see it as the tilt of the contour. Where does this
  correlation come from physically? (More central background ⇔ less signal:
  the θ-slope *is* the separation lever.)
- Compare slice vs. profile separation in both scans. Which parameter
  "absorbs" the correlation?
- Bonus: what changes if `alpha_theta` is fixed to its true value 0.75?
  σ(µ) with and without the systematic — variance subtraction as the price
  of systematics.

# Exercises

Take-home / deep-dive questions for the 1-hour course (see `COURSE.md` for the
guided arc). Organized by course part, then "beyond".

Run any step with `./run.sh` (full pipeline) or individually:
`root -b -q make_toys.cpp`, `unbinned.cpp`, `analysis.cpp`, `histfactory.cpp`, `likelihood.cpp`.

## Part 2 — Binned template fit (`make_toys.cpp`, `analysis.cpp`, `histfactory.cpp`)

6. **Weight closure.** The simulator promises `Σ sig_w = Ns`, `Σ bkg_w = Nb`.
   Add Σw² accumulation to `make_toys.cpp`, show that the printed sums sit
   within √(Σw²) of the expectations, and compute the effective sample size
   N_eff = (Σw)²/Σw² per component.
7. **Vary vs. manual.** Reproduce `h_bkg_theta_up` "by hand": a second event
   loop filling a histogram with
   `toy::bkg_weight(eta, phi, pt, mass, kThetaNom + kThetaDelta)` as weight.
   Bin-by-bin identical? Then explain why Vary is still preferable for ~50
   systematic sources (event loop count, I/O).
8. **Shape vs. normalization.** The `theta` variations change the background
   *integral* as well as its shape. Rescale `h_bkg_theta_up/down` to the
   nominal integral before fitting and compare the fitted `alpha_theta` —
   what changes, and which convention did this toy choose?
9. **Binning.** 4 and 16 bins instead of 8: plot the fitted θ uncertainty
   vs. bin count. Why does it saturate?
10. **Boundaries and Vary.** Weight variations never move events across the
    selection. Construct a variation on `pt` (a ±3% "energy scale") that
    *does* move events across the `pt > 20` cut, apply it via Vary, and
    confirm the varied histograms pick up the boundary migration.

## Part 3 — Likelihood anatomy (`likelihood.cpp`)

11. **HESSE from the scan.** Read σ(µ) and σ(α_θ) off the red profile curves
    at ΔNLL = 0.5 and compare with the printed HESSE errors. Where, if
    anywhere, do they disagree, and does the α_θ curve look parabolic?
12. **Ellipse vs. contour.** Take the covariance matrix from
    `histfactory.cpp`'s `RooFitResult`, draw the 1σ/2σ error ellipses on top of
    `likelihood_contour.pdf`. What difference would you have to look for to
    convict the NLL of non-parabolicity?
13. **Fixed systematic.** In `likelihood.cpp`, re-fit with `alpha_theta` fixed
    at 0.75. By what factor does σ(µ) shrink? Check against
    √(1 − ρ²) from the correlation — why does that relation hold?
14. **Constraint visibility.** Rebuild the NLL without the constraint terms
    (`createNLL(*data, RooFit::GlobalObservables(...))` games, or simply fix
    alpha and compare). Where in the α_θ scan would the likelihood live if the
    data had *no* shape sensitivity? Which side wins here – data or prior –
    and at what binning would that flip (cf. exercise 9)?
15. **Profile of a physical parameter.** µ is the POI, not θ. Produce the
    profiled ΔNLL(θ) — i.e. profile in α_θ as a function of the *physical*
    nuisance value — and read a 95% CL interval on θ. What actually changed
    relative to exercise 11?

## Beyond

16. **Pull validation.** Loop: 50 toy datasets (vary the data seed in
    `make_toys.cpp`), fit each in both Part 1 and Part 2 style, histogram the
    pulls of the respective POIs. Are they unit Gaussians? If not, what is
    the leading suspect given `kNSim`?
17. **Bias vs. MC statistics.** Halve `kNSim` and repeat the Part-2 pull
    study. What fails first: larger σ_θ, bias, or unstable fits? Explain the
    role of `ActivateStatError()` — remove it in `histfactory.cpp` and show the pulls
    come out wrong (too wide or too narrow? predict first).
18. **Workspace anatomy.** After `histfactory.cpp`, dump `w.allVars()` from
    `results/meas_combined_meas_model.root` and classify every ingredient:
    POI, nuisance, constraint nominal, `gamma_stat_*` MC-stat parameters,
    observables. Sketch the model as n_b ~ Pois(µ s_b + γ_b b_b(α_θ)).
19. **Wrong systematic size.** Produce `theta_up/down` at ±2·kThetaDelta while
    `histfactory.cpp` still maps back with kThetaDelta. What do you get for θ̂, and what
    does this teach about the meaning of the HistoSys ±1σ convention?
20. **Interpolation bias.** Generate data at kThetaTrue = 0.8 (α_true = −0.5).
    HistFactory linearly interpolates between your three templates — does the
    `FlexibleInterpVar` reproduce the true exp(−θ·|η|) shape at α = −0.5?
    Quantify the per-bin discrepancy.
21. **Second nuisance.** Add a nuisance β scaling the signal cos(2φ)
    modulation (0.4 → 0.3/0.5) and histogram `phi_fold` as a second
    HistFactory channel. Do µ and the two nuisances stay separately
    constrained? Where does the model rely on channel complementarity?
22. **Discovery.** Compute Z for background-only rejection: a profile
    likelihood ratio in µ (you now know exactly what that is from Part 3),
    or `RooStats::AsymptoticCalculator` on the workspace. Re-derive it after
    doubling kNSig — does Z scale like √(N_sig)?
