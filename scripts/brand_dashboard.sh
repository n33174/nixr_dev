#!/bin/bash

# ==============================================================================
# NIX labs DASHBOARD BRANDING SCRIPT
# Automates the branding of the ESPHome v3 Web Interface for NIX labs
# ==============================================================================

# Configuration
SOURCE_URL="https://oi.esphome.io/v3/www.js"
TARGET_FILE="static/www.js"
TEMP_FILE="static/www_new.js"

# Brand Colors
NIX_ORANGE="#FF9100"
NIX_ORANGE_RGB="255,145,0"
NIX_BACKGROUND_DARK="#030712" # Tailwind Gray-950
NIX_SURFACE_DARK="#111827"    # Tailwind Gray-900
NIX_DIVIDER_DARK="#1f2937"    # Tailwind Gray-800

# Utility to log status
log() {
    echo -e "\033[1;33m[NIX Brand]\033[0m $1"
}

# 1. Download latest source
log "Downloading latest ESPHome v3 source from $SOURCE_URL..."
curl -L -o "$TEMP_FILE" "$SOURCE_URL"

if [ $? -ne 0 ]; then
    log "\033[1;31mERROR: Failed to download source. Check your internet connection.\033[0m"
    exit 1
fi

# 2. Apply NIX Identity (Sed Replacements)
log "Applying NIX labs branding..."

# --- PRIMARY COLORS ---
# ESPHome Blue (#03a9f4 / rgb(3,169,244)) -> Nix Orange
sed -i '' "s/#03a9f4/$NIX_ORANGE/Ig" "$TEMP_FILE"
sed -i '' "s/rgb(3,169,244)/rgb($NIX_ORANGE_RGB)/Ig" "$TEMP_FILE"
sed -i '' "s/rgba(3,169,244/rgba($NIX_ORANGE_RGB/Ig" "$TEMP_FILE"

# --- BACKGROUND COLORS ---
# White (#ffffff / #fff) -> Tailwind Gray-950
sed -i '' "s/#ffffff/$NIX_BACKGROUND_DARK/Ig" "$TEMP_FILE"
sed -i '' "s/#fff/$NIX_BACKGROUND_DARK/Ig" "$TEMP_FILE"
# Off-white (#fafafa) -> Gray-900 (Surface)
sed -i '' "s/#fafafa/$NIX_SURFACE_DARK/Ig" "$TEMP_FILE"
# Dividers/Margins (#eeeeee) -> Gray-800
sed -i '' "s/#e0e0e0/$NIX_DIVIDER_DARK/Ig" "$TEMP_FILE"
sed -i '' "s/rgba(255,255,255/rgba(3,7,18/Ig" "$TEMP_FILE"

# --- TEXT & UI ELEMENTS ---
# Ensure secondary text is Zinc/Slate friendly
sed -i '' "s/#727272/#94a3b8/Ig" "$TEMP_FILE"

# --- FONT STACKS ---
# Replace Roboto with Inter/System fonts across all styles
sed -i '' "s/font-family:Roboto/font-family:Inter,-apple-system,system-ui,sans-serif/Ig" "$TEMP_FILE"
sed -i '' "s/font-family:\"Roboto\"/font-family:\"Inter\",-apple-system,system-ui,sans-serif/Ig" "$TEMP_FILE"

# --- CHART & GRAPH COLORS ---
# Replace common chart colors (Blues/Greens) with Orange/Grays
# ESPHome often uses rgb(3, 169, 244) or similar for charts
sed -i '' "s/rgb(3,169,244)/rgb(255,145,0)/Ig" "$TEMP_FILE"
sed -i '' "s/rgba(3,169,244/rgba(255,145,0/Ig" "$TEMP_FILE"

# --- ICON & COMPONENT COLORS ---
# Target CSS variables inside web components
sed -i '' "s/--primary-color:#03a9f4/--primary-color:#FF9100/Ig" "$TEMP_FILE"
sed -i '' "s/--paper-item-icon-color:#44739e/--paper-item-icon-color:#FF9100/Ig" "$TEMP_FILE"

# --- BRANDING REPLACEMENT ---
# Optional: Replace the logo link or text if found
sed -i '' "s/https:\/\/esphome.io/https:\/\/nixlabs.com.au/Ig" "$TEMP_FILE"

# 3. Finalize
mv "$TEMP_FILE" "$TARGET_FILE"
log "\033[1;32mSUCCESS: Dashboard branded! Target saved to $TARGET_FILE\033[0m"
