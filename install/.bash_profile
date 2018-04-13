export PS1="[ \w --] "
export TERM=xterm-new
echo "Welcome to the Dash"
alias startf="sudo systemctl start dash_frontend"
alias stopf="sudo systemctl stop dash_frontend"
alias startb="sudo systemctl start dash_backend"
alias stopb="sudo systemctl stop dash_backend"
alias statf="sudo systemctl status dash_frontend"
alias statb="sudo systemctl status dash_backend"
