echo "";echo "";echo "";
echo "================================================";
now=$(date +"%T"); echo "Compile at $now"
echo "================================================";
cd /mpi;
mpicc $1 -o lab.o &> compile.o;
cat compile.o;
echo "================================================";
now=$(date +"%T"); echo "Run at $now"
echo "================================================";
mpirun -np $2 lab.o; rm lab.o; rm compile.o;
