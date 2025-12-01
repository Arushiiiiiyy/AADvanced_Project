#!/bin/bash

# Navigate to data directory
cd "$(dirname "$0")"

echo "📥 Downloading SNAP datasets..."
echo ""

# Create directory for SNAP datasets
mkdir -p snap_datasets
cd snap_datasets

# ========== SMALL DATASETS (Quick to test) ==========

echo "1️⃣  Downloading Email-Eu-core (~1K nodes, ~25K edges)..."
wget -q --show-progress https://snap.stanford.edu/data/email-Eu-core. txt. gz
gunzip -f email-Eu-core. txt.gz

echo "2️⃣  Downloading CA-GrQc (~5K nodes, ~14K edges)..."
wget -q --show-progress https://snap. stanford.edu/data/ca-GrQc.txt.gz
gunzip -f ca-GrQc.txt.gz

# ========== MEDIUM DATASETS ==========

echo "3️⃣  Downloading Facebook Combined (~4K nodes, ~88K edges)..."
wget -q --show-progress https://snap. stanford.edu/data/facebook_combined.txt.gz
gunzip -f facebook_combined.txt. gz

echo "4️⃣  Downloading Wiki-Vote (~7K nodes, ~100K edges)..."
wget -q --show-progress https://snap. stanford.edu/data/wiki-Vote.txt.gz
gunzip -f wiki-Vote.txt. gz

echo "5️⃣  Downloading p2p-Gnutella08 (~6K nodes, ~20K edges)..."
wget -q --show-progress https://snap. stanford.edu/data/p2p-Gnutella08. txt.gz
gunzip -f p2p-Gnutella08.txt.gz

# ========== LARGER DATASETS (Optional - takes longer) ==========

read -p "📦 Download larger datasets?  (CA-HepTh, CA-AstroPh) [y/N]: " -n 1 -r
echo
if [[ $REPLY =~ ^[Yy]$ ]]; then
    echo "6️⃣  Downloading CA-HepTh (~9K nodes, ~25K edges)..."
    wget -q --show-progress https://snap.stanford.edu/data/ca-HepTh.txt. gz
    gunzip -f ca-HepTh.txt. gz
    
    echo "7️⃣  Downloading CA-AstroPh (~18K nodes, ~198K edges)..."
    wget -q --show-progress https://snap.stanford.edu/data/ca-AstroPh.txt. gz
    gunzip -f ca-AstroPh.txt. gz
fi

cd .. 

echo ""
echo "✅ Download complete!"
echo "📁 Files saved in: gen_tc/snap_datasets/"
echo ""
ls -lh snap_datasets/*. txt

echo ""
echo "🧹 Cleaning up downloaded files (removing comments)..."

# Remove comment lines starting with #
for file in snap_datasets/*.txt; do
    if [ -f "$file" ]; then
        echo "  Processing $file..."
        grep -v '^#' "$file" > "${file}. tmp"
        mv "${file}. tmp" "$file"
    fi
done

echo "✅ All datasets ready to use!"
