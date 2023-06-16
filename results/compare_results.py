import polars as pl 

scipy_df: pl.DataFrame = (
    pl.scan_csv(
        source="./scipy.csv", 
    )
    .select(("X", "Y", "Tau", "Time"))
    .with_columns(
        ( pl.col("Time") / 1_000_000 ).keep_name() 
    )
    .melt(
        id_vars=("X", "Y"), 
        variable_name="Key", 
        value_name="Scipy", 
    )
    .collect() 
)

cpp_df: pl.DataFrame = (
    pl.scan_csv(
        source="./cpp.csv", 
    )
    .select(("X", "Y", "Tau", "Time"))
    .with_columns(
        ( pl.col("Time") / 1_000_000 ).keep_name() 
    )
    .melt(
        id_vars=("X", "Y"), 
        variable_name="Key", 
        value_name="CPP", 
    )
    .collect() 
)

comp_df: pl.DataFrame = (
    scipy_df
    .join(
        other=cpp_df, 
        on=("X", "Y", "Key"), 
        how="outer", 
    )
    .with_columns([
        ( pl.col("CPP") - pl.col("Scipy") ).alias("Difference"), 
        ( pl.col("CPP") / pl.col("Scipy") ).abs().alias("Ratio"), 
    ])
)
print(comp_df) 
comp_df.write_csv("./versus.csv")

# comp_col: str = "Difference"
comp_col: str = "Ratio" 
threshold: float = 0.0 if comp_col == "Difference" else 100.0
print(
    comp_df
    .groupby("Key")
    .agg([
        pl.col(comp_col).mean().alias("Mean"), 
        pl.col(comp_col).median().alias("Median"), 
        pl.col(comp_col).max().alias("Max"), 
        pl.col(comp_col).min().alias("Min"), 
        ( pl.col(comp_col) > threshold ).sum().alias("n_positive") 
    ])
    .with_columns(
        pl
        .when(pl.col("Key") == "Tau")
        .then(None) 
        .otherwise(pl.col("n_positive"))
        .alias("n_positive") 
    )
    .sort(by="Key") 
)

"""
Difference: 
Tau: want these to be 0 
Time: want to be as negative as possible 

Ratio: 
Tau: want these to be close to 1.0 
Time: want to be as small as possible 

original output 
┌──────┬─────────────┬─────────────┬──────────┬────────────┬────────────┐
│ Key  ┆ Mean        ┆ Median      ┆ Max      ┆ Min        ┆ n_positive │
│ ---  ┆ ---         ┆ ---         ┆ ---      ┆ ---        ┆ ---        │
│ str  ┆ f64         ┆ f64         ┆ f64      ┆ f64        ┆ u32        │
╞══════╪═════════════╪═════════════╪══════════╪════════════╪════════════╡
│ Time ┆ -213.789904 ┆ -360.879425 ┆ 0.289159 ┆ -512.03657 ┆ 19         │
│ Tau  ┆ -0.000174   ┆ 1.1251e-9   ┆ 0.016621 ┆ -0.031508  ┆ 56         │
└──────┴─────────────┴─────────────┴──────────┴────────────┴────────────┘

moving pairs to own function 
┌──────┬─────────────┬─────────────┬──────────┬────────────┬────────────┐
│ Key  ┆ Mean        ┆ Median      ┆ Max      ┆ Min        ┆ n_positive │
│ ---  ┆ ---         ┆ ---         ┆ ---      ┆ ---        ┆ ---        │
│ str  ┆ f64         ┆ f64         ┆ f64      ┆ f64        ┆ u32        │
╞══════╪═════════════╪═════════════╪══════════╪════════════╪════════════╡
│ Tau  ┆ -0.000174   ┆ 1.1251e-9   ┆ 0.016621 ┆ -0.031508  ┆ 56         │
│ Time ┆ -213.869737 ┆ -361.017031 ┆ 0.706374 ┆ -511.51644 ┆ 11         │
└──────┴─────────────┴─────────────┴──────────┴────────────┴────────────┘

multithreading 
┌──────┬─────────────┬─────────────┬──────────┬────────────┬────────────┐
│ Key  ┆ Mean        ┆ Median      ┆ Max      ┆ Min        ┆ n_positive │
│ ---  ┆ ---         ┆ ---         ┆ ---      ┆ ---        ┆ ---        │
│ str  ┆ f64         ┆ f64         ┆ f64      ┆ f64        ┆ u32        │
╞══════╪═════════════╪═════════════╪══════════╪════════════╪════════════╡
│ Tau  ┆ -0.000174   ┆ 1.1251e-9   ┆ 0.016621 ┆ -0.031508  ┆ 56         │
│ Time ┆ -213.780439 ┆ -360.919541 ┆ 0.373446 ┆ -511.88349 ┆ 26         │
└──────┴─────────────┴─────────────┴──────────┴────────────┴────────────┘

refreshed scipy (so scipy improves) 
┌──────┬────────────┬─────────────┬──────────┬──────────────┬────────────┐
│ Key  ┆ Mean       ┆ Median      ┆ Max      ┆ Min          ┆ n_positive │
│ ---  ┆ ---        ┆ ---         ┆ ---      ┆ ---          ┆ ---        │
│ str  ┆ f64        ┆ f64         ┆ f64      ┆ f64          ┆ u32        │
╞══════╪════════════╪═════════════╪══════════╪══════════════╪════════════╡
│ Tau  ┆ -0.000174  ┆ 1.1251e-9   ┆ 0.016621 ┆ -0.031508    ┆ 56         │
│ Time ┆ -155.90856 ┆ -215.120396 ┆ 0.625817 ┆ -3161.482283 ┆ 37         │
└──────┴────────────┴─────────────┴──────────┴──────────────┴────────────┘

scipy was cheating! we had penalty of reading in data as well 
┌──────┬─────────────┬─────────────┬───────────┬──────────────┬────────────┐
│ Key  ┆ Mean        ┆ Median      ┆ Max       ┆ Min          ┆ n_positive │
│ ---  ┆ ---         ┆ ---         ┆ ---       ┆ ---          ┆ ---        │
│ str  ┆ f64         ┆ f64         ┆ f64       ┆ f64          ┆ u32        │
╞══════╪═════════════╪═════════════╪═══════════╪══════════════╪════════════╡
│ Tau  ┆ -0.000174   ┆ 1.1251e-9   ┆ 0.016621  ┆ -0.031508    ┆ null       │
│ Time ┆ -157.852608 ┆ -217.798445 ┆ -0.011999 ┆ -3162.463185 ┆ 0          │
└──────┴─────────────┴─────────────┴───────────┴──────────────┴────────────┘

using faster numpy setup (Ratio) 
┌──────┬──────────┬──────────┬──────────┬──────────┬────────────┐
│ Key  ┆ Mean     ┆ Median   ┆ Max      ┆ Min      ┆ n_positive │
│ ---  ┆ ---      ┆ ---      ┆ ---      ┆ ---      ┆ ---        │
│ str  ┆ f64      ┆ f64      ┆ f64      ┆ f64      ┆ u32        │
╞══════╪══════════╪══════════╪══════════╪══════════╪════════════╡
│ Tau  ┆ 1.129589 ┆ 1.0      ┆ 9.862275 ┆ 0.414215 ┆ null       │
│ Time ┆ 0.346564 ┆ 0.349911 ┆ 0.629224 ┆ 0.000534 ┆ 0          │
└──────┴──────────┴──────────┴──────────┴──────────┴────────────┘

with charger ... 
┌──────┬──────────┬──────────┬──────────┬──────────┬────────────┐
│ Key  ┆ Mean     ┆ Median   ┆ Max      ┆ Min      ┆ n_positive │
│ ---  ┆ ---      ┆ ---      ┆ ---      ┆ ---      ┆ ---        │
│ str  ┆ f64      ┆ f64      ┆ f64      ┆ f64      ┆ u32        │
╞══════╪══════════╪══════════╪══════════╪══════════╪════════════╡
│ Tau  ┆ 1.129589 ┆ 1.0      ┆ 9.862275 ┆ 0.414215 ┆ null       │
│ Time ┆ 0.296173 ┆ 0.295859 ┆ 0.577528 ┆ 0.000487 ┆ 0          │
└──────┴──────────┴──────────┴──────────┴──────────┴────────────┘
"""