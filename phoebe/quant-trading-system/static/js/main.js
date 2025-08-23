// 量化交易系统主要JavaScript文件

// 全局变量
let loadingOverlay = null;

// 页面加载完成后执行
document.addEventListener('DOMContentLoaded', function() {
    initializeApp();
});

// 初始化应用
function initializeApp() {
    // 创建加载遮罩层
    createLoadingOverlay();
    
    // 初始化工具提示
    initializeTooltips();
    
    // 绑定全局事件
    bindGlobalEvents();
    
    console.log('量化交易系统初始化完成');
}

// 创建加载遮罩层
function createLoadingOverlay() {
    loadingOverlay = document.createElement('div');
    loadingOverlay.className = 'loading-overlay';
    loadingOverlay.style.display = 'none';
    loadingOverlay.innerHTML = `
        <div class="loading-spinner">
            <div class="spinner-border text-primary" role="status">
                <span class="visually-hidden">Loading...</span>
            </div>
            <div class="mt-2">
                <span id="loadingText">加载中...</span>
            </div>
        </div>
    `;
    document.body.appendChild(loadingOverlay);
}

// 显示加载状态
function showLoading(message = '加载中...') {
    if (loadingOverlay) {
        const loadingText = loadingOverlay.querySelector('#loadingText');
        if (loadingText) {
            loadingText.textContent = message;
        }
        loadingOverlay.style.display = 'flex';
    }
}

// 隐藏加载状态
function hideLoading() {
    if (loadingOverlay) {
        loadingOverlay.style.display = 'none';
    }
}

// 显示警告消息
function showAlert(type, message, duration = 5000) {
    const alertContainer = document.getElementById('alertContainer');
    if (!alertContainer) {
        console.error('Alert container not found');
        return;
    }
    
    const alertId = 'alert-' + Date.now();
    const alertHtml = `
        <div id="${alertId}" class="alert alert-${type} alert-dismissible fade show" role="alert">
            <i class="fas fa-${getAlertIcon(type)} me-2"></i>
            ${message}
            <button type="button" class="btn-close" data-bs-dismiss="alert"></button>
        </div>
    `;
    
    alertContainer.insertAdjacentHTML('beforeend', alertHtml);
    
    // 自动移除警告
    if (duration > 0) {
        setTimeout(() => {
            const alertElement = document.getElementById(alertId);
            if (alertElement) {
                const bsAlert = new bootstrap.Alert(alertElement);
                bsAlert.close();
            }
        }, duration);
    }
}

// 获取警告图标
function getAlertIcon(type) {
    const icons = {
        'success': 'check-circle',
        'danger': 'exclamation-triangle',
        'warning': 'exclamation-circle',
        'info': 'info-circle',
        'primary': 'bell',
        'secondary': 'bell'
    };
    return icons[type] || 'bell';
}

// 初始化工具提示
function initializeTooltips() {
    const tooltipTriggerList = [].slice.call(document.querySelectorAll('[data-bs-toggle="tooltip"]'));
    tooltipTriggerList.map(function (tooltipTriggerEl) {
        return new bootstrap.Tooltip(tooltipTriggerEl);
    });
}

// 绑定全局事件
function bindGlobalEvents() {
    // ESC键关闭模态框
    document.addEventListener('keydown', function(e) {
        if (e.key === 'Escape') {
            const modals = document.querySelectorAll('.modal.show');
            modals.forEach(modal => {
                const bsModal = bootstrap.Modal.getInstance(modal);
                if (bsModal) {
                    bsModal.hide();
                }
            });
        }
    });
    
    // 防止表单重复提交
    document.addEventListener('submit', function(e) {
        const form = e.target;
        if (form.dataset.submitting === 'true') {
            e.preventDefault();
            return false;
        }
        form.dataset.submitting = 'true';
        
        // 5秒后重置状态
        setTimeout(() => {
            form.dataset.submitting = 'false';
        }, 5000);
    });
}

// 格式化数字
function formatNumber(num, decimals = 2) {
    if (typeof num !== 'number') {
        num = parseFloat(num) || 0;
    }
    return num.toLocaleString('zh-CN', {
        minimumFractionDigits: decimals,
        maximumFractionDigits: decimals
    });
}

// 格式化货币
function formatCurrency(amount, symbol = '¥') {
    return symbol + formatNumber(amount, 2);
}

// 格式化百分比
function formatPercentage(value, decimals = 2) {
    return formatNumber(value, decimals) + '%';
}

// 获取颜色类名（根据值的正负）
function getValueColorClass(value) {
    if (value > 0) return 'text-success';
    if (value < 0) return 'text-danger';
    return 'text-muted';
}

// AJAX请求封装
function apiRequest(url, options = {}) {
    const defaultOptions = {
        method: 'GET',
        headers: {
            'Content-Type': 'application/json',
        },
        credentials: 'same-origin'
    };
    
    const finalOptions = { ...defaultOptions, ...options };
    
    return fetch(url, finalOptions)
        .then(response => {
            if (!response.ok) {
                throw new Error(`HTTP error! status: ${response.status}`);
            }
            return response.json();
        })
        .catch(error => {
            console.error('API request failed:', error);
            throw error;
        });
}

// 刷新数据
function refreshData() {
    showLoading('正在刷新数据...');
    location.reload();
}

// 导出数据
function exportData() {
    showAlert('info', '导出功能开发中...', 3000);
}

// 复制到剪贴板
function copyToClipboard(text) {
    if (navigator.clipboard && window.isSecureContext) {
        navigator.clipboard.writeText(text).then(() => {
            showAlert('success', '已复制到剪贴板', 2000);
        }).catch(err => {
            console.error('复制失败:', err);
            showAlert('danger', '复制失败', 2000);
        });
    } else {
        // 兼容性处理
        const textArea = document.createElement('textarea');
        textArea.value = text;
        textArea.style.position = 'fixed';
        textArea.style.left = '-999999px';
        textArea.style.top = '-999999px';
        document.body.appendChild(textArea);
        textArea.focus();
        textArea.select();
        
        try {
            document.execCommand('copy');
            showAlert('success', '已复制到剪贴板', 2000);
        } catch (err) {
            console.error('复制失败:', err);
            showAlert('danger', '复制失败', 2000);
        } finally {
            textArea.remove();
        }
    }
}

// 下载文件
function downloadFile(url, filename) {
    const link = document.createElement('a');
    link.href = url;
    link.download = filename;
    link.style.display = 'none';
    document.body.appendChild(link);
    link.click();
    document.body.removeChild(link);
}

// 日期格式化
function formatDate(date, format = 'YYYY-MM-DD') {
    if (!(date instanceof Date)) {
        date = new Date(date);
    }
    
    if (isNaN(date.getTime())) {
        return '';
    }
    
    const year = date.getFullYear();
    const month = String(date.getMonth() + 1).padStart(2, '0');
    const day = String(date.getDate()).padStart(2, '0');
    const hours = String(date.getHours()).padStart(2, '0');
    const minutes = String(date.getMinutes()).padStart(2, '0');
    const seconds = String(date.getSeconds()).padStart(2, '0');
    
    return format
        .replace('YYYY', year)
        .replace('MM', month)
        .replace('DD', day)
        .replace('HH', hours)
        .replace('mm', minutes)
        .replace('ss', seconds);
}

// 获取相对时间
function getRelativeTime(date) {
    if (!(date instanceof Date)) {
        date = new Date(date);
    }
    
    if (isNaN(date.getTime())) {
        return '';
    }
    
    const now = new Date();
    const diffMs = now - date;
    const diffSecs = Math.floor(diffMs / 1000);
    const diffMins = Math.floor(diffSecs / 60);
    const diffHours = Math.floor(diffMins / 60);
    const diffDays = Math.floor(diffHours / 24);
    
    if (diffDays > 0) {
        return `${diffDays}天前`;
    } else if (diffHours > 0) {
        return `${diffHours}小时前`;
    } else if (diffMins > 0) {
        return `${diffMins}分钟前`;
    } else {
        return '刚刚';
    }
}

// 防抖函数
function debounce(func, wait, immediate) {
    let timeout;
    return function executedFunction(...args) {
        const later = () => {
            timeout = null;
            if (!immediate) func(...args);
        };
        const callNow = immediate && !timeout;
        clearTimeout(timeout);
        timeout = setTimeout(later, wait);
        if (callNow) func(...args);
    };
}

// 节流函数
function throttle(func, limit) {
    let inThrottle;
    return function() {
        const args = arguments;
        const context = this;
        if (!inThrottle) {
            func.apply(context, args);
            inThrottle = true;
            setTimeout(() => inThrottle = false, limit);
        }
    };
}

// 表格排序
function sortTable(table, column, isNumeric = false) {
    const tbody = table.querySelector('tbody');
    const rows = Array.from(tbody.querySelectorAll('tr'));
    
    rows.sort((a, b) => {
        const aVal = a.cells[column].textContent.trim();
        const bVal = b.cells[column].textContent.trim();
        
        if (isNumeric) {
            return parseFloat(aVal.replace(/[^\d.-]/g, '')) - parseFloat(bVal.replace(/[^\d.-]/g, ''));
        } else {
            return aVal.localeCompare(bVal);
        }
    });
    
    rows.forEach(row => tbody.appendChild(row));
}

// 图表主题配置
const chartTheme = {
    colors: ['#007bff', '#28a745', '#ffc107', '#dc3545', '#17a2b8', '#6f42c1', '#fd7e14'],
    fontFamily: '"Segoe UI", Tahoma, Geneva, Verdana, sans-serif',
    responsive: true,
    maintainAspectRatio: false,
    plugins: {
        legend: {
            position: 'bottom',
            labels: {
                usePointStyle: true,
                padding: 20
            }
        },
        tooltip: {
            backgroundColor: 'rgba(0, 0, 0, 0.8)',
            titleColor: 'white',
            bodyColor: 'white',
            borderColor: 'rgba(255, 255, 255, 0.1)',
            borderWidth: 1,
            cornerRadius: 8,
            displayColors: true
        }
    },
    scales: {
        x: {
            grid: {
                color: 'rgba(0, 0, 0, 0.1)'
            }
        },
        y: {
            grid: {
                color: 'rgba(0, 0, 0, 0.1)'
            }
        }
    }
};

// 全局错误处理
window.addEventListener('error', function(e) {
    console.error('全局错误:', e.error);
    showAlert('danger', '系统遇到错误，请刷新页面重试', 5000);
});

// 网络状态监听
window.addEventListener('online', function() {
    showAlert('success', '网络连接已恢复', 3000);
});

window.addEventListener('offline', function() {
    showAlert('warning', '网络连接已断开', 0);
});

// 导出常用函数到全局
window.TradingSystem = {
    showLoading,
    hideLoading,
    showAlert,
    formatNumber,
    formatCurrency,
    formatPercentage,
    getValueColorClass,
    apiRequest,
    copyToClipboard,
    downloadFile,
    formatDate,
    getRelativeTime,
    debounce,
    throttle,
    sortTable,
    chartTheme
};
