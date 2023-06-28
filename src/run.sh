GRAPH=(dblp google stanford)
K_DBLP=(40 35 30 25 20)
K_GOOGLE=(40 35 30 25)
K_STANFORD=(40)

if [[ $# -ne 1 || $1 != "baseline" && $1 != "sweep" ]]; then
    echo "Usage: ./run.sh [baseline|sweep]"
    exit 1
fi

MAIN="./kvcc_$1"
RESULT_DIR="../results"
LOG_DIR="../logs"

if [ ! -d $RESULT_DIR ]; then
    mkdir $RESULT_DIR
fi

if [ ! -d $LOG_DIR ]; then
    mkdir $LOG_DIR
fi

round=1

dateTime=$(date +"%m%d_%H%M%S")
logFileName="$LOG_DIR/$1_$dateTime.log"
echo "Logging to $logFileName"

for ((i=0; i<$round; i++)); do
    echo "Round $i"
    for g in ${GRAPH[@]}; do
        K=()
        if [ $g == "dblp" ]; then
            K=(${K_DBLP[@]})
        elif [ $g == "google" ]; then
            K=(${K_GOOGLE[@]})
        elif [ $g == "stanford" ]; then
            K=(${K_STANFORD[@]})
        fi
        for k in ${K[@]}; do
            echo "Running $g with k=$k"
            outputFileName="$RESULT_DIR/${g}_k$k.txt"
            echo "Output to $outputFileName"
            $MAIN -g $g -k $k -o $outputFileName >> $logFileName
        done
    done
done