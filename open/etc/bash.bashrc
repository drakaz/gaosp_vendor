##### BASHRC #####
##################

echo '  ____       _                  '
echo ' / ___| __ _| | __ ___  ___   _ '
echo '| |  _ / _` | |/ _` \ \/ / | | |'
echo '| |_| | (_| | | (_| |>  <| |_| |'
echo ' \____|\__,_|_|\__,_/_/\_\\__, |'
echo '                          |___/ ..... by drakaz'
echo ' '


# Options diverses
force_color_prompt=yes

# Prompt
#PS1='${debian_chroot:+($debian_chroot)}\[\033[01;32m\]\u@\h\[\033[00m\]:\[\033[01;34m\]\w\[\033[00m\]\$'
PS1='${debian_chroot:+($debian_chroot)}\[\033[01;32m\][root@galaxy\[\033[01;31m\]:\[\033[01;34m\]\w]\[\033[00m\]'

# Alias
alias ll='ls -l'
alias la='ls -A'

