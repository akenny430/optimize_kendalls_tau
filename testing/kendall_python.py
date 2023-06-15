from pathlib import Path 
import scipy.stats as st 
import time 
from typing import (
    NamedTuple, 
    Optional, 
)

import numpy as np 
import polars as pl 

_DATA_PATH: Path = Path("..", "data", "data") 
_FILE_LIST: list[str] = [f"{x:02}" for x in range(1, 16)] 

class KendallResult(NamedTuple): 
    """
    Way of storing results 
    """
    X: str 
    Y: str 
    Tau: Optional[float] 
    N: int 
    Time: float 

    def __repr__(self) -> str:
        if self.Tau is None: 
            return f"({self.X}, {self.Y}): Unable to compute Kendall's Tau!"
        return f"({self.X}, {self.Y}), {self.N:>4,}: {int( round(self.Time, 0) ):>15,}ns"

def compute_corr_kendall(x: np.ndarray, y: np.ndarray) -> Optional[float]: 
    """
    Compute Kendall's Tau (variation "b")
    """
    tau: Optional[float] = None 

    try: 
        valid = np.isfinite(x) & np.isfinite(y)
        if not valid.all(): 
            tau = st.kendalltau(
                x=x[valid], 
                y=y[valid], 
                variant="b", 
            )[0] 
        else: 
            tau = st.kendalltau(
                x=x, 
                y=y, 
                variant="b", 
            )[0] 
    except: 
        pass 

    return tau



# looping through each combination and computing Kendall's Tau 
kendall_result_list: list[KendallResult] = [] 
for i, n1 in enumerate(_FILE_LIST): 
    df1: pl.DataFrame = (
        pl.scan_csv(source=_DATA_PATH / f"{n1}.csv")
        .rename({"X": "x"}) 
        .collect() 
    )
    for j, n2 in enumerate(_FILE_LIST): 
        if i >= j: 
            continue

        df2: pl.DataFrame = (
            pl.scan_csv(source=_DATA_PATH / f"{n2}.csv")
            .rename({"X": "y"}) 
            .collect() 
        )

        whole_df: pl.DataFrame = (
            df1
            .join(
                other=df2, 
                on="Date", 
                how="outer", 
            )
        )

        t0: float = time.perf_counter_ns() 
        tau: Optional[float] = compute_corr_kendall(
            x=whole_df.get_column("x").to_numpy(), 
            y=whole_df.get_column("y").to_numpy(), 
        )
        tf: float = time.perf_counter_ns()
        elapsed_time: float = float(tf - t0)

        result: KendallResult = (
            KendallResult(
                X=n1, 
                Y=n2, 
                Tau=tau, 
                N=whole_df.shape[0],
                Time=elapsed_time,
            )
        )
        print(result) 
        kendall_result_list.append(result)

python_df: pl.DataFrame = (
    pl.DataFrame(
        data=kendall_result_list, 
        schema=KendallResult._fields, 
    )
)
python_df.write_csv(file=Path.cwd() / ".." / "results" / "scipy.csv")