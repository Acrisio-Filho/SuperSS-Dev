#!/bin/bash

# Base Directory
dir=/home/acrisio/projects/Linux\ Builds

as=Auth\ Server
ls=Login\ Server
ms=Message\ Server
rs=Rank\ Server
gs=Game\ Server
smart=Smart-Calculator

if [ "$#" -eq 1 -a "$1" = "kill" ]
then
	echo "Kill sessions tmux"
	tmux send-keys -t "${as}" "exit" ENTER
	tmux send-keys -t "${ls}" "exit" ENTER
	tmux send-keys -t "${ms}" "exit" ENTER
	tmux send-keys -t "${rs}" "exit" ENTER
	tmux send-keys -t "${gs}" "exit" ENTER
	tmux kill-session -t "${smart}"
	tmux ls
	exit 0
fi

# Smart Calculator lib
tmux new-session -s "${smart}" -c "/home/acrisio/projects/Node/${smart}/" -d 'node smart.js'

# Auth Server
tmux new-session -s "${as}" -c "${dir}/${as}/" -d ./auth

# Login Server
tmux new-session -s "${ls}" -c "${dir}/${ls}/" -d ./login

# Message Server
tmux new-session -s "${ms}" -c "${dir}/${ms}/" -d ./msn

# Rank Server
tmux new-session -s "${rs}" -c "${dir}/${rs}/" -d ./rank

if [ "$#" -eq 1 -a "$1" = "all" ]
then
	# Game Server
	tmux new-session -s "${gs}" -c "${dir}/${gs}/" -d ./game
fi

# Show sessions createds
tmux ls
