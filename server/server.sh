# Serves NodeMCU's file requests

if test $# -lt 1
then
    echo "No arguments."
    exit 1
else
    case $1 in
        G)
            cat current_conditions ;;
        default)
            echo "I don't know that argument." ;;
    esac
fi
