import pandas as pd 
import plotnine as pn 
import polars as pl 

# df: pl.DataFrame = (
df: pd.DataFrame = (
    pl.scan_csv(source="./versus.csv")
    .melt(
        id_vars=("X", "Y", "Key"), 
        variable_name="Source", 
        value_name="Value", 
    )
    .collect() 
    .to_pandas() 
)
print(df) 

for (key, source), gbdf in df.groupby(["Key", "Source"]): 
    print(f"({key}, {source})")
    # print(gbdf)
    if key == "Time": 
        if source != "Ratio": 
            continue
        pass 
    elif key == "Tau": 
        if source != "Difference": 
            continue

    gbplot: pn.ggplot = (
        pn.ggplot(
            data=gbdf, 
            mapping=pn.aes(x="Value"), 
        )
        + pn.geom_histogram(fill="green", bins=200) 
        + pn.theme_bw() 
    )
    gbplot.save(
        filename=f"./plots/{key}_{source}.pdf" 
    )