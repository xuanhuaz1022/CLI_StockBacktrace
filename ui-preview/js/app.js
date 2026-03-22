// App JavaScript

document.addEventListener('DOMContentLoaded', function() {
    // Initialize app
    initApp();
});

function initApp() {
    initNavigation();
    initThemeToggle();
    initSidebarToggle();
    initMobileMenu();
    initForms();
    initButtons();
    initNotifications();
    initTooltips();
    initDropdowns();
    initTabs();
    initModals();
    initDrawers();
}

// Navigation
function initNavigation() {
    const navLinks = document.querySelectorAll('.nav-link');
    const navItems = document.querySelectorAll('.nav-item');
    const currentPage = document.getElementById('currentPage');
    const pages = document.querySelectorAll('.page');

    navLinks.forEach(link => {
        link.addEventListener('click', function(e) {
            e.preventDefault();
            const page = this.getAttribute('data-page');
            navigateToPage(page);
        });
    });

    navItems.forEach(item => {
        item.addEventListener('click', function() {
            const page = this.getAttribute('data-page');
            navigateToPage(page);
        });
    });

    function navigateToPage(page) {
        // Update nav links
        navLinks.forEach(link => {
            link.classList.remove('active');
            if (link.getAttribute('data-page') === page) {
                link.classList.add('active');
            }
        });

        // Update nav items
        navItems.forEach(item => {
            item.classList.remove('active');
            if (item.getAttribute('data-page') === page) {
                item.classList.add('active');
            }
        });

        // Update pages
        pages.forEach(p => {
            p.style.display = 'none';
            if (p.id === `page-${page}`) {
                p.style.display = 'block';
            }
        });

        // Update breadcrumb
        const pageNames = {
            'dashboard': '仪表盘',
            'data': '数据管理',
            'strategy': '策略管理',
            'backtest': '回测管理',
            'results': '结果分析'
        };
        if (currentPage) {
            currentPage.textContent = pageNames[page] || page;
        }

        // Close mobile menu
        const sidebar = document.getElementById('sidebar');
        if (sidebar && window.innerWidth < 768) {
            sidebar.classList.remove('active');
        }
    }
}

// Theme Toggle
function initThemeToggle() {
    const themeToggle = document.getElementById('themeToggle');
    if (!themeToggle) return;

    // Check for saved theme preference or default to light
    const savedTheme = localStorage.getItem('theme') || 'light';
    document.documentElement.setAttribute('data-theme', savedTheme);

    themeToggle.addEventListener('click', function() {
        const currentTheme = document.documentElement.getAttribute('data-theme');
        const newTheme = currentTheme === 'light' ? 'dark' : 'light';
        
        document.documentElement.setAttribute('data-theme', newTheme);
        localStorage.setItem('theme', newTheme);

        // Update icon
        updateThemeIcon(newTheme);
    });

    // Initialize icon
    updateThemeIcon(savedTheme);
}

function updateThemeIcon(theme) {
    const themeToggle = document.getElementById('themeToggle');
    if (!themeToggle) return;

    const sunIcon = `<svg width="20" height="20" viewBox="0 0 20 20" fill="none" xmlns="http://www.w3.org/2000/svg">
        <path d="M10 2V4M10 16V18M18 10H16M4 10H2M15.6569 4.34315L14.2426 5.75736M5.75736 14.2426L4.34315 15.6569M15.6569 15.6569L14.2426 14.2426M5.75736 5.75736L4.34315 4.34315" stroke="currentColor" stroke-width="2" stroke-linecap="round"/>
        <circle cx="10" cy="10" r="4" stroke="currentColor" stroke-width="2"/>
    </svg>`;

    const moonIcon = `<svg width="20" height="20" viewBox="0 0 20 20" fill="none" xmlns="http://www.w3.org/2000/svg">
        <path d="M17.293 13.293L18.707 14.707C19.098 14.317 19.098 13.683 18.707 13.293L17.293 13.293ZM16 10C16 13.3137 13.3137 16 10 16V18C14.4183 18 18 14.4183 18 10H16ZM10 16C6.68629 16 4 13.3137 4 10H2C2 14.4183 5.58172 18 10 18V16ZM4 10C4 6.68629 6.68629 4 10 4V2C5.58172 2 2 5.58172 2 10H4ZM10 4C13.3137 4 16 6.68629 16 10H18C18 5.58172 14.4183 2 10 2V4Z" fill="currentColor"/>
        <path d="M10 2C10 2 8 5 8 8C8 11 10 14 10 14C10 14 12 11 12 8C12 5 10 2 10 2Z" fill="currentColor"/>
    </svg>`;

    themeToggle.innerHTML = theme === 'light' ? sunIcon : moonIcon;
}

// Sidebar Toggle
function initSidebarToggle() {
    const sidebarToggle = document.getElementById('sidebarToggle');
    const sidebar = document.getElementById('sidebar');
    
    if (!sidebarToggle || !sidebar) return;

    sidebarToggle.addEventListener('click', function() {
        sidebar.classList.toggle('collapsed');
    });
}

// Mobile Menu
function initMobileMenu() {
    // Create mobile menu toggle if it doesn't exist
    if (!document.querySelector('.mobile-menu-toggle')) {
        const headerLeft = document.querySelector('.header-left');
        if (headerLeft) {
            const mobileToggle = document.createElement('button');
            mobileToggle.className = 'mobile-menu-toggle';
            mobileToggle.innerHTML = `<svg width="20" height="20" viewBox="0 0 20 20" fill="none" xmlns="http://www.w3.org/2000/svg">
                <path d="M3 5H17M3 10H17M3 15H17" stroke="currentColor" stroke-width="2" stroke-linecap="round"/>
            </svg>`;
            headerLeft.insertBefore(mobileToggle, headerLeft.firstChild);

            mobileToggle.addEventListener('click', function() {
                const sidebar = document.getElementById('sidebar');
                if (sidebar) {
                    sidebar.classList.toggle('active');
                }
            });
        }
    }

    // Close sidebar when clicking outside on mobile
    document.addEventListener('click', function(e) {
        const sidebar = document.getElementById('sidebar');
        const mobileToggle = document.querySelector('.mobile-menu-toggle');
        
        if (window.innerWidth < 768 && sidebar && !sidebar.contains(e.target) && !mobileToggle.contains(e.target)) {
            sidebar.classList.remove('active');
        }
    });
}

// Forms
function initForms() {
    const forms = document.querySelectorAll('.form');

    forms.forEach(form => {
        // Form validation
        form.addEventListener('submit', function(e) {
            e.preventDefault();
            if (validateForm(form)) {
                handleFormSubmit(form);
            }
        });

        // Real-time validation
        const inputs = form.querySelectorAll('input, select, textarea');
        inputs.forEach(input => {
            input.addEventListener('blur', function() {
                validateInput(this);
            });

            input.addEventListener('input', function() {
                if (this.classList.contains('error')) {
                    validateInput(this);
                }
            });
        });
    });
}

function validateForm(form) {
    let isValid = true;
    const inputs = form.querySelectorAll('input[required], select[required], textarea[required]');

    inputs.forEach(input => {
        if (!validateInput(input)) {
            isValid = false;
        }
    });

    return isValid;
}

function validateInput(input) {
    const value = input.value.trim();
    const isRequired = input.hasAttribute('required');
    const type = input.type;
    let isValid = true;
    let errorMessage = '';

    // Required validation
    if (isRequired && !value) {
        isValid = false;
        errorMessage = '此字段为必填项';
    }

    // Email validation
    if (type === 'email' && value) {
        const emailRegex = /^[^\s@]+@[^\s@]+\.[^\s@]+$/;
        if (!emailRegex.test(value)) {
            isValid = false;
            errorMessage = '请输入有效的邮箱地址';
        }
    }

    // Number validation
    if (type === 'number' && value) {
        const num = parseFloat(value);
        if (isNaN(num)) {
            isValid = false;
            errorMessage = '请输入有效的数字';
        }

        const min = input.getAttribute('min');
        const max = input.getAttribute('max');
        
        if (min !== null && num < parseFloat(min)) {
            isValid = false;
            errorMessage = `最小值为 ${min}`;
        }

        if (max !== null && num > parseFloat(max)) {
            isValid = false;
            errorMessage = `最大值为 ${max}`;
        }
    }

    // Update input state
    if (isValid) {
        input.classList.remove('error');
        const errorElement = input.parentElement.querySelector('.form-error');
        if (errorElement) {
            errorElement.remove();
        }
    } else {
        input.classList.add('error');
        let errorElement = input.parentElement.querySelector('.form-error');
        if (!errorElement) {
            errorElement = document.createElement('div');
            errorElement.className = 'form-error';
            input.parentElement.appendChild(errorElement);
        }
        errorElement.textContent = errorMessage;
    }

    return isValid;
}

function handleFormSubmit(form) {
    // Show loading state
    const submitButton = form.querySelector('button[type="submit"]');
    if (submitButton) {
        const originalText = submitButton.textContent;
        submitButton.disabled = true;
        submitButton.textContent = '提交中...';

        // Simulate form submission
        setTimeout(() => {
            submitButton.disabled = false;
            submitButton.textContent = originalText;
            showNotification('success', '提交成功', '表单已成功提交');
            form.reset();
        }, 1000);
    }
}

// Buttons
function initButtons() {
    const buttons = document.querySelectorAll('.btn');

    buttons.forEach(button => {
        button.addEventListener('click', function(e) {
            // Handle button clicks
            if (this.classList.contains('btn-danger')) {
                // Confirm dangerous actions
                const action = this.textContent.trim();
                if (confirm(`确定要执行 "${action}" 操作吗？此操作不可撤销。`)) {
                    showNotification('success', '操作成功', '操作已成功执行');
                } else {
                    e.preventDefault();
                }
            }
        });
    });
}

// Notifications
function initNotifications() {
    // Create notification container if it doesn't exist
    if (!document.querySelector('.notification-container')) {
        const container = document.createElement('div');
        container.className = 'notification-container';
        document.body.appendChild(container);
    }
}

function showNotification(type, title, message, duration = 3000) {
    const container = document.querySelector('.notification-container');
    if (!container) return;

    const notification = document.createElement('div');
    notification.className = `notification ${type}`;

    const icons = {
        success: `<svg class="notification-icon" width="20" height="20" viewBox="0 0 20 20" fill="none" xmlns="http://www.w3.org/2000/svg">
            <path d="M10 0C4.48 0 0 4.48 0 10C0 15.52 4.48 20 10 20C15.52 20 20 15.52 20 10C20 4.48 15.52 0 10 0ZM8 15L3 10L4.41 8.59L8 12.17L15.59 4.58L17 6L8 15Z" fill="currentColor"/>
        </svg>`,
        warning: `<svg class="notification-icon" width="20" height="20" viewBox="0 0 20 20" fill="none" xmlns="http://www.w3.org/2000/svg">
            <path d="M10 0L18.66 16H1.34L10 0ZM10 6V10M10 12V14" stroke="currentColor" stroke-width="2" stroke-linecap="round" stroke-linejoin="round"/>
        </svg>`,
        error: `<svg class="notification-icon" width="20" height="20" viewBox="0 0 20 20" fill="none" xmlns="http://www.w3.org/2000/svg">
            <path d="M10 0C4.48 0 0 4.48 0 10C0 15.52 4.48 20 10 20C15.52 20 20 15.52 20 10C20 4.48 15.52 0 10 0ZM15 13H5V11H15V13Z" fill="currentColor"/>
        </svg>`,
        info: `<svg class="notification-icon" width="20" height="20" viewBox="0 0 20 20" fill="none" xmlns="http://www.w3.org/2000/svg">
            <path d="M10 0C4.48 0 0 4.48 0 10C0 15.52 4.48 20 10 20C15.52 20 20 15.52 20 10C20 4.48 15.52 0 10 0ZM11 15H9V9H11V15ZM11 7H9V5H11V7Z" fill="currentColor"/>
        </svg>`
    };

    notification.innerHTML = `
        ${icons[type] || icons.info}
        <div class="notification-content">
            <div class="notification-title">${title}</div>
            <div class="notification-message">${message}</div>
        </div>
        <button class="notification-close">&times;</button>
    `;

    container.appendChild(notification);

    // Add close functionality
    const closeBtn = notification.querySelector('.notification-close');
    closeBtn.addEventListener('click', () => {
        removeNotification(notification);
    });

    // Auto remove after duration
    setTimeout(() => {
        removeNotification(notification);
    }, duration);
}

function removeNotification(notification) {
    notification.classList.add('removing');
    setTimeout(() => {
        notification.remove();
    }, 300);
}

// Tooltips
function initTooltips() {
    const tooltips = document.querySelectorAll('.tooltip');

    tooltips.forEach(tooltip => {
        tooltip.addEventListener('mouseenter', function() {
            const content = this.querySelector('.tooltip-content');
            if (content) {
                content.style.opacity = '1';
                content.style.visibility = 'visible';
            }
        });

        tooltip.addEventListener('mouseleave', function() {
            const content = this.querySelector('.tooltip-content');
            if (content) {
                content.style.opacity = '0';
                content.style.visibility = 'hidden';
            }
        });
    });
}

// Dropdowns
function initDropdowns() {
    const dropdowns = document.querySelectorAll('.dropdown');

    dropdowns.forEach(dropdown => {
        const toggle = dropdown.querySelector('.dropdown-toggle');
        
        toggle.addEventListener('click', function(e) {
            e.stopPropagation();
            dropdown.classList.toggle('active');
        });

        const items = dropdown.querySelectorAll('.dropdown-item');
        items.forEach(item => {
            item.addEventListener('click', function(e) {
                e.preventDefault();
                dropdown.classList.remove('active');
                // Handle dropdown item click
            });
        });
    });

    // Close dropdowns when clicking outside
    document.addEventListener('click', function() {
        dropdowns.forEach(dropdown => {
            dropdown.classList.remove('active');
        });
    });
}

// Tabs
function initTabs() {
    const tabsContainers = document.querySelectorAll('.tabs');

    tabsContainers.forEach(container => {
        const tabs = container.querySelectorAll('.tab');
        const contents = container.parentElement.querySelectorAll('.tab-content');

        tabs.forEach(tab => {
            tab.addEventListener('click', function() {
                const target = this.getAttribute('data-target');

                // Update tab states
                tabs.forEach(t => t.classList.remove('active'));
                this.classList.add('active');

                // Update content states
                contents.forEach(content => {
                    content.classList.remove('active');
                    if (content.id === target) {
                        content.classList.add('active');
                    }
                });
            });
        });
    });
}

// Modals
function initModals() {
    const modalTriggers = document.querySelectorAll('[data-modal]');
    const modals = document.querySelectorAll('.modal');

    modalTriggers.forEach(trigger => {
        trigger.addEventListener('click', function() {
            const modalId = this.getAttribute('data-modal');
            const modal = document.getElementById(modalId);
            if (modal) {
                openModal(modal);
            }
        });
    });

    modals.forEach(modal => {
        const closeBtn = modal.querySelector('.modal-close');
        const overlay = modal.parentElement;

        if (closeBtn) {
            closeBtn.addEventListener('click', () => closeModal(modal));
        }

        overlay.addEventListener('click', (e) => {
            if (e.target === overlay) {
                closeModal(modal);
            }
        });
    });

    // Close modals with Escape key
    document.addEventListener('keydown', (e) => {
        if (e.key === 'Escape') {
            modals.forEach(modal => {
                if (modal.parentElement.classList.contains('active')) {
                    closeModal(modal);
                }
            });
        }
    });
}

function openModal(modal) {
    const overlay = modal.parentElement;
    overlay.classList.add('active');
    document.body.style.overflow = 'hidden';
}

function closeModal(modal) {
    const overlay = modal.parentElement;
    overlay.classList.remove('active');
    document.body.style.overflow = '';
}

// Drawers
function initDrawers() {
    const drawerTriggers = document.querySelectorAll('[data-drawer]');
    const drawers = document.querySelectorAll('.drawer');

    drawerTriggers.forEach(trigger => {
        trigger.addEventListener('click', function() {
            const drawerId = this.getAttribute('data-drawer');
            const drawer = document.getElementById(drawerId);
            if (drawer) {
                openDrawer(drawer);
            }
        });
    });

    drawers.forEach(drawer => {
        const closeBtn = drawer.querySelector('.drawer-close');
        const overlay = drawer.parentElement;

        if (closeBtn) {
            closeBtn.addEventListener('click', () => closeDrawer(drawer));
        }

        overlay.addEventListener('click', (e) => {
            if (e.target === overlay) {
                closeDrawer(drawer);
            }
        });
    });

    // Close drawers with Escape key
    document.addEventListener('keydown', (e) => {
        if (e.key === 'Escape') {
            drawers.forEach(drawer => {
                if (drawer.parentElement.classList.contains('active')) {
                    closeDrawer(drawer);
                }
            });
        }
    });
}

function openDrawer(drawer) {
    const overlay = drawer.parentElement;
    overlay.classList.add('active');
    document.body.style.overflow = 'hidden';
}

function closeDrawer(drawer) {
    const overlay = drawer.parentElement;
    overlay.classList.remove('active');
    document.body.style.overflow = '';
}

// Utility functions
function debounce(func, wait) {
    let timeout;
    return function executedFunction(...args) {
        const later = () => {
            clearTimeout(timeout);
            func(...args);
        };
        clearTimeout(timeout);
        timeout = setTimeout(later, wait);
    };
}

function throttle(func, limit) {
    let inThrottle;
    return function(...args) {
        if (!inThrottle) {
            func.apply(this, args);
            inThrottle = true;
            setTimeout(() => inThrottle = false, limit);
        }
    };
}

// Handle window resize
let resizeTimeout;
window.addEventListener('resize', () => {
    clearTimeout(resizeTimeout);
    resizeTimeout = setTimeout(() => {
        // Handle responsive adjustments
        const sidebar = document.getElementById('sidebar');
        if (window.innerWidth >= 768 && sidebar) {
            sidebar.classList.remove('active');
        }
    }, 250);
});

// Initialize app when DOM is ready
if (document.readyState === 'loading') {
    document.addEventListener('DOMContentLoaded', initApp);
} else {
    initApp();
}