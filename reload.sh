# Reloads all programs (example script)

echo "Reloading..."

kill -SIGUSR1 $KITTY_PID
pkill waybar
waybar > /dev/null 2>&1 & disown

makoctl reload

pkill hyprpaper
hyprpaper > /dev/null 2>&1 & disown
