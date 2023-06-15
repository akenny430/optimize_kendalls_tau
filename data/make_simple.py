import polars as pl 

df: pl.DataFrame = pl.DataFrame(data={"X": [12.0, 2.0, 1.0, 12.0, 2.0]})
df.write_csv(file="./data/s1.csv")

df: pl.DataFrame = pl.DataFrame(data={"X": [1.0, 4.0, 7.0, 1.0, 0.0]})
df.write_csv(file="./data/s2.csv")