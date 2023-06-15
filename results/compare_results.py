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
    .with_columns(
        ( pl.col("CPP") - pl.col("Scipy") ).alias("Difference")
    )
)
print(comp_df) 
comp_df.write_csv("./versus.csv")
"""
Tau: want these to be 0 
Time: want to be as negative as possible 
"""

print(
    comp_df
    .groupby("Key")
    .agg([
        pl.col("Difference").mean().alias("Mean"), 
        pl.col("Difference").median().alias("Median"), 
        pl.col("Difference").max().alias("Max"), 
        pl.col("Difference").min().alias("Min"), 
        ( pl.col("Difference") > 0.0 ).sum().alias("n_positive") 
    ])
)