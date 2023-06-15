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