# Reloads all programs (example script)

echo "Reloading..."

kill -SIGUSR1 $KITTY_PID
pkill waybar
waybar > /dev/null 2>&1 & disown

makoctl reload

pkill hyprpaper
hyprpaper > /dev/null 2>&1 & disown

# appends colors to hyprlock file, since it doesnt support including
cat ~/.config/hypr/colors.conf ~/.config/hypr/hyprlock-default.conf > ~/.config/hypr/hyprlock.conf
