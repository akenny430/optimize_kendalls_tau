from datetime import date as Date 

import numpy as np 
import pandas as pd 
import polars as pl 

_RNG: np.random.Generator = np.random.default_rng(seed=0) 

_DATES: pd.Series = pd.date_range(start=Date(2018, 1, 1), end=Date(2022, 12, 31), freq="B")



for i in range(1, 11): 
    df: pl.DataFrame = (
        pl.DataFrame(
            data={
                "Date": _DATES, 
                "X": _RNG.normal(loc=0.0 + i*0.25, scale=1.0 + i*0.05, size=len(_DATES))
            }, 
            # schema={
            #     "Date": pl.Date, 
            #     "X": pl.Float64, 
            # }
        )
        .with_columns(
            pl.col("Date").cast(pl.Date).keep_name()
        )
    )
    df.write_csv(f"./data/{i:02}.csv") 

# now making some where there are null values 
for i in range(11, 16): 
    gaussian_data: np.ndarray = _RNG.normal(loc=0.0 + i*0.25, scale=1.0 + i*0.05, size=len(_DATES))
    which_to_set_nan: np.ndarray = _RNG.choice(a=len(_DATES), size=250, replace=False)
    gaussian_data[which_to_set_nan] = np.nan 
    df: pl.DataFrame = (
        pl.DataFrame(
            data={
                "Date": _DATES, 
                "X": gaussian_data
            }, 
        )
        .with_columns([
            pl.col("Date").cast(pl.Date).keep_name(), 
            pl.when(pl.col("X").is_nan())
            .then(None)
            .otherwise(pl.col("X"))
            .keep_name() 
        ])
    )
    df.write_csv(f"./data/{i:02}.csv") 