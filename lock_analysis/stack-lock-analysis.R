library(ggplot2)
library(reshape2)

pdf("stack-lock-analysis.pdf")

results <- read.table("results.dat", header=TRUE)


molten <- melt(data=results, id.vars=c("threads", "lock"), measure.vars="time")
data_median <- dcast(data=molten, formula="threads + lock ~ variable", fun.aggregate=median)


m <- ggplot(data=results, aes(x=factor(threads), y=time, fill=lock))
m <- m + geom_line(data=data_median, aes(x=threads, y=time, color=lock))
m <- m + geom_boxplot(notch=TRUE, outlier.shape = NA, position="identity", alpha=0.5)
m <- m + scale_x_discrete("Number of Threads")
m <- m + scale_y_continuous("Time per update [ns]")
m <- m + theme_bw(20)
print(m)

dev.off()
