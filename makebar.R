# Creates a concatenated bar-plot
# Andre Anjos <Andre.dos.Anjos@cern.ch>

mkbar <- function (fname, main) {
  d <- read.csv(fname, header=FALSE);
  d <- d[1:49,]; #Cuts duplication of ICC measurements
  
  h <- d[,3];
  dim(h) <- c(7,length(h)/7);
  h <- t(h);
  n <- as.vector(unique(d[,2]));
  leg <- as.vector(unique(d[,1]));
  col <- rainbow(dim(h)[1],start=0.1,end=0.3);
  barplot(h, beside=TRUE, 
          xlab="Number of Threads", ylab="Total Execution Time (us)",
          main=main, names.arg=as.character(n), col=col);
  xloc <- par("usr")[3] - par("usr")[1];
  xloc <- par("usr")[1]*(-1.1);
  yloc <- par("usr")[4] - par("usr")[2];
  yloc <- par("usr")[4] - 0.4*yloc;
  legend(xloc, yloc, legend=leg, fill=col, yjust=0);
}

mkbarstr <- function (fname, main) {
  d <- read.csv(fname, header=FALSE);
  
  h <- d[,3];
  dim(h) <- c(7,length(h)/7);
  h <- t(h);
  n <- as.vector(unique(d[,2]));
  leg <- as.vector(unique(d[,1]));
  col <- rainbow(dim(h)[1],start=0.1,end=0.3);
  barplot(h, beside=TRUE, 
          xlab="Number of Threads", ylab="Total Execution Time (us)",
          main=main, names.arg=as.character(n), col=col);
  xloc <- par("usr")[3] - par("usr")[1];
  xloc <- par("usr")[1]*(-1.1);
  yloc <- par("usr")[4] - par("usr")[2];
  yloc <- par("usr")[4] - 0.35*yloc;
  legend(xloc, yloc, legend=leg, fill=col, yjust=0);
}

png("list.png");
mkbar("list.csv", "Standard list allocation performance");
dev.off();
png("vector.png");
mkbar("vector.csv", "Standard vector allocation performance");
dev.off();
png("string.png");
mkbarstr("string.csv", "Standard string allocation performance");
dev.off();

